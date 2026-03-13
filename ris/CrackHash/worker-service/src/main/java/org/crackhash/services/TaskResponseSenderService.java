package org.crackhash.services;

import lombok.AllArgsConstructor;
import lombok.extern.slf4j.Slf4j;
import org.crackhash.model.requests.CrackHashWorkerResponse;
import org.crackhash.services.util.UriHandlerService;
import org.springframework.stereotype.Service;
import org.springframework.web.client.RestClient;

import java.util.List;

@Service
@AllArgsConstructor
@Slf4j
public class TaskResponseSenderService {
    private final RestClient restClient;
    private final UriHandlerService uriHandlerService;

    public void send(List<String> matchingWords, String requestId, int partNumber) {
        CrackHashWorkerResponse requestBody = new CrackHashWorkerResponse();
        requestBody.setRequestId(requestId);
        requestBody.setPartNumber(partNumber);
        var answers = new CrackHashWorkerResponse.Answers();
        answers.getWords().addAll(matchingWords);
        requestBody.setAnswers(answers);
        //TODO: fix application-type issue
        var response = restClient.patch().uri(uriHandlerService.getManagerTaskResultUri()).body(requestBody).retrieve().toBodilessEntity();
        log.info("Response was: {}", response.getStatusCode());
    }
}
