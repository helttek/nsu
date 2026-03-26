package org.crackhash.config;

import lombok.RequiredArgsConstructor;
import org.springframework.amqp.core.*;
import org.springframework.amqp.rabbit.config.SimpleRabbitListenerContainerFactory;
import org.springframework.amqp.rabbit.connection.ConnectionFactory;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.amqp.support.converter.JacksonJsonMessageConverter;
import org.springframework.amqp.support.converter.MarshallingMessageConverter;
import org.springframework.amqp.support.converter.MessageConverter;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.oxm.jaxb.Jaxb2Marshaller;

@Configuration
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
        return BindingBuilder
                .bind(workerQueue)
                .to(workerExchange)
                .with(appProperties.getRabbitmq().getWorker().getRoutingKey());
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
        return BindingBuilder
                .bind(managerQueue)
                .to(managerExchange)
                .with(appProperties.getRabbitmq().getManager().getRoutingKey());
    }

    @Bean
    public MessageConverter jsonMarshaller() {
        return new JacksonJsonMessageConverter();
    }

    @Bean
    public RabbitTemplate rabbitTemplate(ConnectionFactory connectionFactory, MessageConverter jsonConverter) {
        RabbitTemplate template = new RabbitTemplate(connectionFactory);
        template.setMessageConverter(jsonConverter);
        return template;
    }

    @Bean
    public SimpleRabbitListenerContainerFactory rabbitListenerContainerFactory(
            ConnectionFactory connectionFactory,
            MessageConverter jsonMessageConverter
    ) {
        SimpleRabbitListenerContainerFactory factory =
                new SimpleRabbitListenerContainerFactory();
        factory.setConnectionFactory(connectionFactory);
        factory.setMessageConverter(jsonMessageConverter);
        factory.setAcknowledgeMode(AcknowledgeMode.MANUAL);
        return factory;
    }

}
