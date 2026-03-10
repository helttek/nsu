package org.crackhash.dto.request;

import lombok.Getter;
import lombok.Setter;

@Getter
@Setter
public class CrackTaskCreationRequest {
    private String hash;

    private int maxLength;
}
