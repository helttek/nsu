package org.crackhash.config;

import lombok.Getter;
import lombok.Setter;
import org.springframework.boot.context.properties.ConfigurationProperties;
import org.springframework.stereotype.Component;

import java.util.List;

@Component
@ConfigurationProperties(prefix = "app")
@Getter
@Setter
public class AppProperties {
    private Worker worker;
    private List<String> alphabet;

    @Getter
    @Setter
    public static class Worker {
        private int count;
    }
}
