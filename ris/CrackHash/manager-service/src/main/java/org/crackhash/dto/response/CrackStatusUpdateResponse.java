package org.crackhash.dto.response;

import lombok.Getter;
import lombok.Setter;
import org.crackhash.dto.response.util.CrackUpdateStatusEnum;

import java.util.List;

@Getter
@Setter
public class CrackStatusUpdateResponse {
    CrackUpdateStatusEnum status;

    List<String> data;
}
