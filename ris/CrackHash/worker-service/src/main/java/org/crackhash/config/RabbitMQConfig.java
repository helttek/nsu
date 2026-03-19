package org.crackhash.config;

import org.crackhash.model.requests.CrackHashManagerRequest;
import org.crackhash.model.requests.CrackHashWorkerResponse;
import org.springframework.amqp.core.Binding;
import org.springframework.amqp.core.BindingBuilder;
import org.springframework.amqp.core.DirectExchange;
import org.springframework.amqp.core.Queue;
import org.springframework.amqp.rabbit.connection.ConnectionFactory;
import org.springframework.amqp.rabbit.core.RabbitTemplate;
import org.springframework.amqp.support.converter.MarshallingMessageConverter;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.oxm.jaxb.Jaxb2Marshaller;

@Configuration
public class RabbitMQConfig {

    @Value("${app.rabbitmq.worker.exchange}")
    private String workerExchangeName;

    @Value("${app.rabbitmq.worker.queue}")
    private String workerQueueName;

    @Value("${app.rabbitmq.worker.routing-key}")
    private String workerRoutingKey;

    @Value("${app.rabbitmq.manager.exchange}")
    private String managerExchangeName;

    @Value("${app.rabbitmq.manager.queue}")
    private String managerQueueName;

    @Value("${app.rabbitmq.manager.routing-key}")
    private String managerRoutingKey;

    @Bean
    public DirectExchange workerExchange() {
        return new DirectExchange(workerExchangeName);
    }

    @Bean
    public Queue workerQueue() {
        return new Queue(workerQueueName, true);
    }

    @Bean
    public Binding workerBinding(Queue workerQueue, DirectExchange workerExchange) {
        return BindingBuilder.bind(workerQueue).to(workerExchange).with(workerRoutingKey);
    }

    @Bean
    public DirectExchange managerExchange() {
        return new DirectExchange(managerExchangeName);
    }

    @Bean
    public Queue managerQueue() {
        return new Queue(managerQueueName, true);
    }

    @Bean
    public Binding managerBinding(Queue managerQueue, DirectExchange managerExchange) {
        return BindingBuilder.bind(managerQueue).to(managerExchange).with(managerRoutingKey);
    }

    @Bean
    public Jaxb2Marshaller xmlMarshaller() {
        Jaxb2Marshaller marshaller = new Jaxb2Marshaller();
        // Tell JAXB which classes it needs to convert to/from XML
        marshaller.setClassesToBeBound(CrackHashWorkerResponse.class, CrackHashManagerRequest.class);
        return marshaller;
    }

    @Bean
    public RabbitTemplate rabbitTemplate(ConnectionFactory connectionFactory, Jaxb2Marshaller xmlMarshaller) {
        RabbitTemplate template = new RabbitTemplate(connectionFactory);
        template.setMessageConverter(new MarshallingMessageConverter(xmlMarshaller));
        return template;
    }
}
