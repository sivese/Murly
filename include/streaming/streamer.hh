#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>

// WebRTC includes
#include "api/create_peerconnection_factory.h"
#include "api/peer_connection_interface.h"
#include "api/media_stream_interface.h"
#include "api/video/video_frame.h"
#include "api/video/video_sink_interface.h"
#include "api/video/video_source_interface.h"
#include "api/scoped_refptr.h"

#include "modules/video_capture/video_capture.h"
#include "modules/audio_device/include/audio_device.h"
#include "pc/video_track_source.h"

#include "rtc_base/thread.h"
#include "rtc_base/ref_counted_object.h"

class WebRTCStreamer {
public:
    WebRTCStreamer();
    ~WebRTCStreamer();

    bool initialize();
    
    bool startStreaming(int camera_index);
    void stopStreaming();

    bool createPeerConnection();
    void handleOffer();
    void handleAnswer();
    void handleIceCandidate(const std::string& candidate, const std::string& sdp_mid, int sdp_mline_index);

    std::function<void(const std::string& sdp)> onOfferCreated;
    std::function<void(const std::string& sdp)> onAnsweredCreated;
    std::function<void(const std::string& candidate, const std::string& sdp_mid, int sdp_mline_index)> onIceCandidateFound;
private:

};