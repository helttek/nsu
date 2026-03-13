package org.crackhash.service;

import lombok.extern.slf4j.Slf4j;
import org.crackhash.config.AppProperties;
import org.crackhash.model.responses.CrackHashManagerRequest;
import org.crackhash.service.util.UriHandlerService;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.web.client.RestClient;

@Service
@Slf4j
public class CrackTaskSenderService {
    private final AppProperties appProperties;
    private final RestClient restClient;
    private final UriHandlerService uriHandlerService;

    @Autowired
    public CrackTaskSenderService(AppProperties appProperties, RestClient restClient, UriHandlerService uriHandlerService) {
        this.appProperties = appProperties;
        this.restClient = restClient;
        this.uriHandlerService = uriHandlerService;
    }

    public void send(String requestId, String hash, int hashMaxLength) {
        log.info("Starting to send the task to workers ({})", appProperties.getWorker().getCount());
        for (int i = 1; i <= appProperties.getWorker().getCount(); i++) {
            CrackHashManagerRequest request = new CrackHashManagerRequest();

            CrackHashManagerRequest.Alphabet alphabet = new CrackHashManagerRequest.Alphabet();
            alphabet.getSymbols().addAll(appProperties.getAlphabet());
            request.setAlphabet(alphabet);
            request.setRequestId(requestId);
            request.setHash(hash);
            request.setPartNumber(i - 1);
            request.setMaxLength(hashMaxLength);
            request.setPartCount(appProperties.getWorker().getCount());

            log.info("Sending to ");
            var response = restClient.post().uri(uriHandlerService.getBaseUri(i) + uriHandlerService.getWORKER_TASK_URI()).body(request).retrieve().toBodilessEntity();
            log.info("Response was: {}", response.getStatusCode());
        }
    }
}
