package org.crackhash.service;

import org.crackhash.util.CrackTaskStatusEnum;
import org.springframework.stereotype.Service;

import java.util.concurrent.ConcurrentHashMap;

@Service
public class CrackTaskStatusTrackerService {
    private final ConcurrentHashMap<String, CrackTaskStatusEnum> hashMap = new ConcurrentHashMap<>();
}
