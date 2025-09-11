#pragma once

#include "api/jsep.h"

class WebRTCStreamer;

class CreateSDPObserver : public webrtc::CreateSessionDescriptionObserver {
public:
    CreateSDPObserver(WebRTCStreamer* streamer);

    void OnSuccess(webrtc::SessionDescriptionInterface* desc) override;
    void OnFailure(webrtc::RTCError error) override;
private:
    WebRTCStreamer* streamer_;
    bool is_offer_;
};

class SetSDPObserver : public webrtc::SetSessionDescriptionObserver {
public:
    void OnSuccess() override;
    void OnFailure(webrtc::RTCError error) override;
};