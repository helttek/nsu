package fit.snake_game.model;

import fit.snake_game.model.events.EventListener;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.InetAddress;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class Server extends NetNode implements EventListener {
    private int UNIQUE_ID = 0;

    private long serverMsgCount = 0;

    private final List<SnakesProto.GamePlayer> players;
    private final List<SnakesProto.GameState.Snake> snakes;


    public Server() throws IOException {

        gameModel = GameModel.getInstance();

        players = gameModel.getPlayers();
        snakes = gameModel.getSnakes();

        gameModel.getNetEventManager().subscribe(this, "receivedPacket");
    }

    public synchronized long getServerMsgCount() {
        return serverMsgCount;
    }

    public synchronized void increaseServerMsgCount() {
        serverMsgCount++;
    }

    @Override
    public void update(String eventType, DatagramPacket receivedPacket) {
        try {
            SnakesProto.GameMessage gameMessage = SnakesProto.GameMessage.parseFrom(Arrays.copyOf(receivedPacket.getData(), receivedPacket.getLength()));

            if (gameMessage.hasJoin()) {
                SnakesProto.GameMessage.JoinMsg msg = gameMessage.getJoin();
                List<SnakesProto.GameState.Coord> coords = gameModel.getGameField().findSnakeInitPlace();
                if (coords != null) {
//                    System.out.println("Players: " + players.size());
                    if (players.isEmpty()) {
                        players.add(createGamePlayer(msg.getName(), UNIQUE_ID, receivedPacket.getAddress().getHostAddress(), receivedPacket.getPort(),
                                SnakesProto.NodeRole.MASTER, 0));

                    } else if (players.size() == 1) {
                        players.add(createGamePlayer(msg.getName(), UNIQUE_ID, receivedPacket.getAddress().getHostAddress(), receivedPacket.getPort(),
                                SnakesProto.NodeRole.DEPUTY, 0));
                        gameModel.sendMessage(receivedPacket.getAddress(), receivedPacket.getPort(), gameModel.createChangeRoleRoleMsg(SnakesProto.NodeRole.DEPUTY));

                    } else {
                        SnakesProto.GamePlayer newPlayer = createGamePlayer(msg.getName(), UNIQUE_ID, receivedPacket.getAddress().getHostAddress(), receivedPacket.getPort(),
                                SnakesProto.NodeRole.NORMAL, 0);

                        for (int i = 0; i < players.size(); i++) {
                            if (players.get(i).getIpAddress().equals(newPlayer.getIpAddress()) && players.get(i).getPort() == newPlayer.getPort() && players.get(i).getRole() == SnakesProto.NodeRole.VIEWER) {
                                players.remove(i);
                                break;
                            }
                        }
                        if (players.size() == 1) {
                            players.add(createGamePlayer(msg.getName(), UNIQUE_ID, receivedPacket.getAddress().getHostAddress(), receivedPacket.getPort(),
                                    SnakesProto.NodeRole.DEPUTY, 0));
                            gameModel.sendMessage(receivedPacket.getAddress(), receivedPacket.getPort(), gameModel.createChangeRoleRoleMsg(SnakesProto.NodeRole.DEPUTY));

                        } else {
                            players.add(newPlayer);
                        }
                    }

                    snakes.add(createSnake(UNIQUE_ID, coords, SnakesProto.GameState.Snake.SnakeState.ALIVE, SnakesProto.Direction.RIGHT));
                    UNIQUE_ID++;
                } else {
                    sendErrorMessage(receivedPacket.getAddress(), receivedPacket.getPort(),
                            "[SERVER_MESSAGE] Error! For player [" + receivedPacket.getAddress().getHostAddress() + " " +
                                    receivedPacket.getPort() + "] is no place on the field!");
                }

                sendConfirmation(receivedPacket.getAddress(), receivedPacket.getPort(), receivedPacket);
            } else if (gameMessage.hasSteer()) {
//                System.out.println(gameMessage);
                SnakesProto.GameMessage.SteerMsg steerMsg = gameMessage.getSteer();
                List<SnakesProto.GameState.Snake> newSnakes = new ArrayList<>();
                int playerId = -1;
//                System.out.println("Received message player: " + receivedPacket.getAddress().getHostAddress() + " " + receivedPacket.getPort());
                for (SnakesProto.GamePlayer gamePlayer : players) {
//                    System.out.println("Player from list: " + gamePlayer.getIpAddress() + " " + gamePlayer.getPort());
                    if (gamePlayer.getIpAddress().equals(receivedPacket.getAddress().getHostAddress())
                            && gamePlayer.getPort() == receivedPacket.getPort()) {
                        playerId = gamePlayer.getId();
                    }
                }
                for (SnakesProto.GameState.Snake snake : snakes) {
                    if (snake.getPlayerId() == playerId) {
                        newSnakes.add(createSnake(snake.getPlayerId(), snake.getPointsList(), snake.getState(),
                                steerMsg.getDirection()));
                    } else {
                        newSnakes.add(createSnake(snake.getPlayerId(), snake.getPointsList(), snake.getState(),
                                snake.getHeadDirection()));
                    }
                }
                snakes.clear();
                snakes.addAll(newSnakes);
                sendConfirmation(receivedPacket.getAddress(), receivedPacket.getPort(), receivedPacket);
            } else if (gameMessage.hasPing()) {
                sendConfirmation(receivedPacket.getAddress(), receivedPacket.getPort(), receivedPacket);
            } else if (gameMessage.hasAck()) {
                gameModel.serverConfirmMessageWithSeq(gameMessage.getMsgSeq());
            }
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }

    private SnakesProto.GamePlayer createGamePlayer(String name, int id, String ipAddress, int port, SnakesProto.NodeRole nodeRole, int score) {
        return SnakesProto.GamePlayer.newBuilder()
                .setName(name)
                .setId(id)
                .setIpAddress(ipAddress)
                .setPort(port)
                .setRole(nodeRole)
                .setType(SnakesProto.PlayerType.HUMAN)
                .setScore(score).build();
    }

    private SnakesProto.GameState.Snake createSnake(int playerId, List<SnakesProto.GameState.Coord> points, SnakesProto.GameState.Snake.SnakeState snakeState,
                                                    SnakesProto.Direction direction) {
        SnakesProto.GameState.Snake.Builder snake = SnakesProto.GameState.Snake.newBuilder();

        snake.setPlayerId(playerId);

        for (int i = 0; i < points.size(); i++) {
            snake.addPoints(i, points.get(i));
        }

        snake.setState(snakeState);
        snake.setHeadDirection(direction);

        return snake.build();
    }

    private void sendErrorMessage(InetAddress inetAddress, int port, String errorMessage) throws IOException {
        byte[] buf = createErrorMsg(errorMessage).toByteArray();
        gameModel.getDatagramSocket().send(new DatagramPacket(buf, buf.length, inetAddress, port));
    }

    private SnakesProto.GameMessage createErrorMsg(String errorMessage) {
        SnakesProto.GameMessage.Builder gameMessage = SnakesProto.GameMessage.newBuilder();

        gameMessage.setError(SnakesProto.GameMessage.ErrorMsg.newBuilder().setErrorMessage(errorMessage).build());

        gameMessage.setMsgSeq(serverMsgCount);
        increaseServerMsgCount();

        return gameMessage.build();
    }
}