package org.crackhash.config;

import lombok.RequiredArgsConstructor;
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
