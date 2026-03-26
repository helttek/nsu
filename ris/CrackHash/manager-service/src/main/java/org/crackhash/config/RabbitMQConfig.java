package org.crackhash.config;

import lombok.Getter;
import lombok.RequiredArgsConstructor;
import org.springframework.amqp.core.Binding;
import org.springframework.amqp.core.BindingBuilder;
import org.springframework.amqp.core.DirectExchange;
import org.springframework.amqp.core.Queue;
import org.springframework.amqp.rabbit.connection.ConnectionFactory;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.amqp.support.converter.JacksonJsonMessageConverter;
import org.springframework.amqp.support.converter.MessageConverter;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
@Getter
@RequiredArgsConstructor
public class RabbitMQConfig {
    private final AppProperties appProperties;

    @Bean
    public DirectExchange workerExchange() {
        return new DirectExchange(appProperties.getRabbitmq().getWorker().getExchange());
    }

    @Bean
    public Queue workerQueue() {
        return new Queue(appProperties.getRabbitmq().getWorker().getQueue(), true);
    }

    @Bean
    public Binding workerBinding(Queue workerQueue, DirectExchange workerExchange) {
        return BindingBuilder.bind(workerQueue).to(workerExchange).with(appProperties.getRabbitmq().getWorker().getRoutingKey());
    }

    @Bean
    public DirectExchange managerExchange() {
        return new DirectExchange(appProperties.getRabbitmq().getManager().getExchange());
    }

    @Bean
    public Queue managerQueue() {
        return new Queue(appProperties.getRabbitmq().getManager().getQueue(), true);
    }

    @Bean
    public Binding managerBinding(Queue managerQueue, DirectExchange managerExchange) {
        return BindingBuilder.bind(managerQueue).to(managerExchange).with(appProperties.getRabbitmq().getManager().getRoutingKey());
    }

    @Bean
    public MessageConverter xmlMarshaller() {
        return new JacksonJsonMessageConverter();
    }

    @Bean
    public RabbitTemplate rabbitTemplate(ConnectionFactory connectionFactory, MessageConverter jsonMarshaller) {
        RabbitTemplate template = new RabbitTemplate(connectionFactory);
        template.setMessageConverter(jsonMarshaller);
        return template;
    }
}

