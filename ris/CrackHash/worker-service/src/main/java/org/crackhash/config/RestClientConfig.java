package org.crackhash.config;

import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.http.HttpHeaders;
import org.springframework.web.client.RestClient;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.http.MediaType;
import org.springframework.http.converter.xml.Jaxb2RootElementHttpMessageConverter;
import org.springframework.web.client.RestClient;

@Configuration
public class RestClientConfig {

    @Bean
    public RestClient config(RestClient.Builder builder) {
        return RestClient.builder()
                .baseUrl("http://manager-service:8080")
                .defaultHeader("Accept", MediaType.APPLICATION_XML_VALUE)
                .build();
    }
}
