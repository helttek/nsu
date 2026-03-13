package org.crackhash.services.util;

import jakarta.annotation.PostConstruct;
import lombok.Getter;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

@Service
@Getter
public class UriHandlerService {
    private String baseUrl;
    @Value("${app.manager.port}")
    private String managerPort;
    private final String managerTaskResultPath = "/internal/api/manager/hash/crack/request";

    @PostConstruct
    public void getBaseUri() {
        baseUrl = "http://crackhash-manager-service-1:" + managerPort;
    }

    public String getManagerTaskResultUri() {
        return baseUrl + managerTaskResultPath;
    }
}
