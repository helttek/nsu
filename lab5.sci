function Lyapunova_continuous_PI()
    n = 4;
    T0 = 1.23;
    T = 0;
    
    K = 0.6375;
    Ti = 5.25;

    S = poly(0, 's');
    W0 = 2 / (1 + S * T0)^n;
    Wl = (1 + 1 / (Ti * S)) * K * W0;
    W = Wl / (1 + Wl);
    disp("W:");
    disp(W);

    A = [zeros(1, 4); eye(4, 4)];
    ac = [0.1061037; 0.9939418; 2.1495357; 3.9658933; 3.2520325];
    A = [A, -ac];
    disp("A:");
    disp(A);

    I = eye(A);
    H = lyap(A, -I, 'c');
    disp("H:");
    disp(H);

    H_spectr = spec(H);
    if min(real(H_spectr)) <= %eps then
        disp("H is not >0");
        abort;
    end

    kappa = max(H_spectr);
    disp("kappa:");
    disp(kappa);
endfunction

function Lyapunova_continuous_PID()
    n = 4;
    T0 = 1.23;
    T = 0;
    
    K = 1;
    Ti = 4.9;
    Td = Ti / 4;
    Tc = Td / 8;

    S = poly(0, 's');
    W0 = 2 / (1 + S * T0)^n;
    Wl = (1 + 1 / (Ti * S) + ((Td * S) / (1 + Tc * S)) ) * K * W0;
    W = Wl / (1 + Wl);
    disp("W:");
    disp(W);

    A = [zeros(1, 5); eye(5, 5)];
    ac = [1.1645754; 8.7379549; 22.338843; 28.049255; 25.20366; 9.7826452];
    A = [A, -ac];
    disp("A:");
    disp(A);

    I = eye(A);
    H = lyap(A, -I, 'c');
    disp("H:");
    disp(H);

    H_spectr = spec(H);
    if min(real(H_spectr)) <= %eps then
        disp("H is not >0");
        abort;
    end

    kappa = max(H_spectr);
    disp("kappa:");
    disp(kappa);
endfunction
