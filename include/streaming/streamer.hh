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

// Forward declarations
class PCObserver;

class WebRTCStreamer {
public:
    // Core functionality
    bool initialize();
    bool createPeerConnection();
    
    bool startStreaming(int camera_index);
    void stopStreaming();

    // SDP handling
    void createOffer();
    void createAnswer();
    void setRemoteDescription(const std::string& sdp, const std::string& type);
    void setLocalDescription(const std::string& sdp, const std::string& type);

    // ICE candidate handling
    void addIceCandidate(const std::string& candidate, const std::string& sdp_mid, int sdp_mline_index);

    // Callback functions for signaling
    std::function<void(const std::string& sdp)> onOfferCreated;
    std::function<void(const std::string& sdp)> onAnsweredCreated;
    std::function<void(const std::string& candidate, const std::string& sdp_mid, int sdp_mline_index)> onIceCandidateFound;
private:
    // WebRTC core components
    webrtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface> peer_connection_factory_;
    webrtc::scoped_refptr<webrtc::PeerConnectionInterface> peer_connection_;
    
    // Threading
    std::unique_ptr<webrtc::Thread> signaling_thread_;
    std::unique_ptr<webrtc::Thread> worker_thread_;
    std::unique_ptr<webrtc::Thread> network_thread_;

    // Video components
    webrtc::scoped_refptr<webrtc::VideoTrackSourceInterface> video_source_;
    
    // Observer
    std::unique_ptr<PCObserver> pc_observer_;
};