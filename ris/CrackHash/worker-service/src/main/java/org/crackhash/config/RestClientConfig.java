package org.crackhash.config;

import lombok.extern.slf4j.Slf4j;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;
import org.springframework.http.HttpStatusCode;
import org.springframework.web.client.RestClient;

@Configuration
@Slf4j
public class RestClientConfig {


    @Bean
    public RestClient config(RestClient.Builder builder) {
        return builder
                .defaultStatusHandler(HttpStatusCode::isError, (request, response) -> {
                    log.info("request: {}, response: {}", request, response);
                })
                .build();
    }
}
