package org.crackhash.dto.response;

import lombok.Getter;
import lombok.Setter;
import org.crackhash.util.CrackTaskStatusEnum;

import java.util.List;

@Getter
@Setter
public class CrackTaskStatusUpdateResponse {
    CrackTaskStatusEnum status;

    List<String> data;
}
