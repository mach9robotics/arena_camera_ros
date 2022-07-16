// #include "stdafx.h"
#include "ArenaApi.h"
#include <algorithm> // for std::find
#include <unistd.h> // for sleep
#include <vector>
#include <chrono>
#include <thread>

#define TAB1 "  "
#define TAB2 "    "
#define TAB3 "      "
#define ERASE_LINE "                            "

#define EXPOSURE_TIME 1000.0
#define PTPSYNC_FRAME_RATE 7.0

// =-=-=-=-=-=-=-=-=-
// =-=- SETTINGS =-=-
// =-=-=-=-=-=-=-=-=-

// Image timeout
//    Timeout for grabbing images (in milliseconds). If no image is available at
//    the end of the timeout, an exception is thrown. The timeout is the maximum
//    time to wait for an image; however, getting an image will return as soon as
//    an image is available, not waiting the full extent of the timeout.
#define TIMEOUT 20000

// number of images to grab
#define NUM_IMAGES 25000


// =-=-=-=-=-=-=-=-=-
// =-=- EXAMPLE -=-=-
// =-=-=-=-=-=-=-=-=-

//
void PTPSyncCamerasAndAcquireImages(Arena::ISystem* pSystem, std::vector<Arena::IDevice*>& devices)
{

    for (size_t i = 0; i < devices.size(); i++)
    {
        Arena::IDevice* pDevice = devices.at(i);
        GenICam::gcstring deviceSerialNumber = Arena::GetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "DeviceSerialNumber");

        std::cout << TAB2 << "Prepare camera " << deviceSerialNumber << "\n";

        // Manually set exposure time
        //    In order to get synchronized images, the exposure time must be
        //    synchronized as well.
        std::cout << TAB3 << "Exposure: ";

        Arena::SetNodeValue<GenICam::gcstring>(
            pDevice->GetNodeMap(),
            "ExposureAuto",
            "Off");

        Arena::SetNodeValue(
            pDevice->GetNodeMap(),
            "ExposureTime",
            EXPOSURE_TIME);

        std::cout << Arena::GetNodeValue<double>(pDevice->GetNodeMap(), "ExposureTime") << "\n";

        // Synchronize devices by enabling PTP
        //    Enabling PTP on multiple devices causes them to negotiate amongst
        //    themselves so that there is a single master device while all the
        //    rest become slaves. The slaves' clocks all synchronize to the
        //    master's clock.
        std::cout << TAB3 << "PTP: ";

        Arena::SetNodeValue(
            pDevice->GetNodeMap(),
            "PtpEnable",
            true);

        std::cout << (Arena::GetNodeValue<bool>(pDevice->GetNodeMap(), "PtpEnable") ? "enabled, " : "disabled, ") <<
            Arena::GetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "PtpStatus") << std::endl;

        // Use max supported packet size. We use transfer control to ensure that only one camera
        // is transmitting at a time.
        std::cout << TAB3 << "StreamAutoNegotiatePacketSize: ";
        Arena::SetNodeValue(pDevice->GetTLStreamNodeMap(), "StreamAutoNegotiatePacketSize", true);
        std::cout << Arena::GetNodeValue<bool>(pDevice->GetTLStreamNodeMap(), "StreamAutoNegotiatePacketSize") << "\n";

        // enable stream packet resend
        std::cout << TAB3 << "StreamPacketResendEnable: ";
        Arena::SetNodeValue(pDevice->GetTLStreamNodeMap(), "StreamPacketResendEnable", true);
        std::cout << Arena::GetNodeValue<bool>(pDevice->GetTLStreamNodeMap(), "StreamPacketResendEnable") << "\n";

        // Set acquisition mode to 'Continuous'
        std::cout << TAB3 << "Set acquisition mode to 'Continuous'\n";
        Arena::SetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "AcquisitionMode", "Continuous");

        //Set acquisition start mode to 'PTPSync'
        // std::cout << TAB3 << "Set acquisition start mode to 'PTPSync'\n";
        // Arena::SetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "AcquisitionStartMode", "Normal");

        // 	Set StreamBufferHandlingMode to 'NewestOnly'
        std::cout << TAB3 << "Set StreamBufferHandlingMode to 'NewestOnly'\n";
        Arena::SetNodeValue<GenICam::gcstring>(pDevice->GetTLStreamNodeMap(), "StreamBufferHandlingMode", "NewestOnly");

        // Set pixel format to Mono8
        Arena::SetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "PixelFormat", "Mono8");
        std::cout << TAB3 << "Set pixel format to 'Mono8' \n";

        int64_t pDeviceLinkSpeed = Arena::GetNodeValue<int64_t>(pDevice->GetNodeMap(), "DeviceLinkSpeed");
        std::cout << TAB3 << "DeviceLinkSpeed = " << pDeviceLinkSpeed << std::endl;

        int64_t pDeviceStreamChannelPacketSize = Arena::GetNodeValue<int64_t>(pDevice->GetNodeMap(), "DeviceStreamChannelPacketSize");
        std::cout << TAB3 << "DeviceStreamChannelPacketSize = " << pDeviceStreamChannelPacketSize << std::endl;


        if (i == 0)
        {
            // Packet Delay
            GenApi::CIntegerPtr pStreamChannelPacketDelay = pDevice->GetNodeMap()->GetNode("GevSCPD");
            pStreamChannelPacketDelay->SetValue(240000);
            std::cout << TAB3 << "GevSCPD: ";
            std::cout << Arena::GetNodeValue<int64_t>(pDevice->GetNodeMap(), "GevSCPD") << "\n";

            // Transmission Delay
            GenApi::CIntegerPtr pStreamChannelFrameTransmissionDelay = pDevice->GetNodeMap()->GetNode("GevSCFTD");
            pStreamChannelFrameTransmissionDelay->SetValue(0);
            std::cout << TAB3 << "GevSCFTD: ";
            std::cout << Arena::GetNodeValue<int64_t>(pDevice->GetNodeMap(), "GevSCFTD") << "\n";
        }

        else if (i == 1)
        {
            // Packet Delay
            GenApi::CIntegerPtr pStreamChannelPacketDelay = pDevice->GetNodeMap()->GetNode("GevSCPD");
            pStreamChannelPacketDelay->SetValue(240000);
            std::cout << TAB3 << "GevSCPD: ";
            std::cout << Arena::GetNodeValue<int64_t>(pDevice->GetNodeMap(), "GevSCPD") << "\n";

            // Transmission Delay
            GenApi::CIntegerPtr pStreamChannelFrameTransmissionDelay = pDevice->GetNodeMap()->GetNode("GevSCFTD");
            pStreamChannelFrameTransmissionDelay->SetValue(80000);
            std::cout << TAB3 << "GevSCFTD: ";
            std::cout << Arena::GetNodeValue<int64_t>(pDevice->GetNodeMap(), "GevSCFTD") << "\n";
        }

        else if (i == 2)
        {
            // Packet Delay
            GenApi::CIntegerPtr pStreamChannelPacketDelay = pDevice->GetNodeMap()->GetNode("GevSCPD");
            pStreamChannelPacketDelay->SetValue(240000);
            std::cout << TAB3 << "GevSCPD: ";
            std::cout << Arena::GetNodeValue<int64_t>(pDevice->GetNodeMap(), "GevSCPD") << "\n";

            // Transmission Delay
            GenApi::CIntegerPtr pStreamChannelFrameTransmissionDelay = pDevice->GetNodeMap()->GetNode("GevSCFTD");
            pStreamChannelFrameTransmissionDelay->SetValue(80000 * 2);
            std::cout << TAB3 << "GevSCFTD: ";
            std::cout << Arena::GetNodeValue<int64_t>(pDevice->GetNodeMap(), "GevSCFTD") << "\n";
        }

        else if (i == 3)
        {
            // Packet Delay
            GenApi::CIntegerPtr pStreamChannelPacketDelay = pDevice->GetNodeMap()->GetNode("GevSCPD");
            pStreamChannelPacketDelay->SetValue(240000);
            std::cout << TAB3 << "GevSCPD: ";
            std::cout << Arena::GetNodeValue<int64_t>(pDevice->GetNodeMap(), "GevSCPD") << "\n";

            // Transmission Delay
            GenApi::CIntegerPtr pStreamChannelFrameTransmissionDelay = pDevice->GetNodeMap()->GetNode("GevSCFTD");
            pStreamChannelFrameTransmissionDelay->SetValue(80000 * 3);
            std::cout << TAB3 << "GevSCFTD: ";
            std::cout << Arena::GetNodeValue<int64_t>(pDevice->GetNodeMap(), "GevSCFTD") << "\n";
        }

        // Frame rate
        // GenApi::CBooleanPtr pAcquisitionFrameRateEnable = pDevice->GetNodeMap()->GetNode("AcquisitionFrameRateEnable");
        // pAcquisitionFrameRateEnable->SetValue(true);

        // GenApi::CFloatPtr pAcquisitionFrameRate = pDevice->GetNodeMap()->GetNode("AcquisitionFrameRate");
        // pAcquisitionFrameRate->SetValue(0.5);

        // PTPSyncFrameRate
        // GenApi::CFloatPtr pPTPSyncFrameRate = pDevice->GetNodeMap()->GetNode("PTPSyncFrameRate");
        // if (pPTPSyncFrameRate != nullptr)
        // {
        //     pPTPSyncFrameRate->SetValue(PTPSYNC_FRAME_RATE);
        // }
    }

    // prepare system
    std::cout << TAB2 << "Prepare system\n";

    // Wait for devices to negotiate their PTP relationship
    //    Before starting any PTP-dependent actions, it is important to wait for
    //    the devices to complete their negotiation; otherwise, the devices may
    //    not yet be synced. Depending on the initial PTP state of each camera,
    //    it can take about 40 seconds for all devices to autonegotiate. Below,
    //    we wait for the PTP status of each device until there is only one
    //    'Master' and the rest are all 'Slaves'. During the negotiation phase,
    //    multiple devices may initially come up as Master so we will wait until
    //    the ptp negotiation completes.
    std::cout << TAB1 << "Wait for devices to negotiate. This can take up to about 40s.\n";


    // start stream
    std::cout << "\n"
        << TAB1 << "Start stream\n";

    for (size_t i = 0; i < devices.size(); i++)
    {
        devices.at(i)->StartStream();
    }


    // get images and check timestamps
    std::cout << TAB1 << "Get images\n";

    for (size_t i = 0; i < NUM_IMAGES; i++)
    {
        for (size_t j = 0; j < devices.size(); j++)
        {
            Arena::IDevice* pDevice = devices.at(j);
            GenICam::gcstring deviceSerialNumber = Arena::GetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "DeviceSerialNumber");

            std::cout << TAB2 << "Image " << i << " from device " << deviceSerialNumber << "\n";

            // Compare timestamps
            //    Scheduling action commands amongst PTP synchronized devices results
            //    in synchronized images with synchronized timestamps.
            std::cout << TAB3 << "Timestamp: ";

            Arena::IImage* pImage = pDevice->GetImage(3000);

            std::cout << pImage->GetTimestamp() << "\n";

            // requeue buffer
            pDevice->RequeueBuffer(pImage);
        }
    }

    // stop stream
    std::cout << TAB1 << "Stop stream\n";

    for (size_t i = 0; i < devices.size(); i++)
    {
        devices.at(i)->StopStream();
    }


}

// =-=-=-=-=-=-=-=-=-
// =- PREPARATION -=-
// =- & CLEAN UP =-=-
// =-=-=-=-=-=-=-=-=-

int main()
{
    // flag to track when an exception has been thrown
    bool exceptionThrown = false;

    std::cout << "Cpp_PTPSync\n";

    try
    {
        // prepare example
        Arena::ISystem* pSystem = Arena::OpenSystem();
        pSystem->UpdateDevices(100);
        auto deviceInfos = pSystem->GetDevices();
        if (deviceInfos.size() < 2)
        {
            if (deviceInfos.size() == 0)
                std::cout << "\nNo camera connected. Example requires at least 2 devices\n";
            else if (deviceInfos.size() == 1)
                std::cout << "\nOnly one device connected. Example requires at least 2 devices\n";

            std::cout << "Press enter to complete\n";

            // clear input
            while (std::cin.get() != '\n')
                continue;

            std::getchar();
            return 0;
        }
        std::vector<Arena::IDevice*> devices;
        for (size_t i = 0; i < deviceInfos.size(); i++)
        {
            devices.push_back(pSystem->CreateDevice(deviceInfos.at(i)));
        }

        std::cout << "Found " << devices.size() << " devices\n";

        // run example
        std::cout << "Commence example\n\n";
        PTPSyncCamerasAndAcquireImages(pSystem, devices);
        std::cout << "\nExample complete\n";

        // clean up example
        for (size_t i = 0; i < devices.size(); i++)
        {
            pSystem->DestroyDevice(devices.at(i));
        }
        Arena::CloseSystem(pSystem);
    }
    catch (GenICam::GenericException& ge)
    {
        std::cout << "\nGenICam exception thrown: " << ge.what() << "\n";
        exceptionThrown = true;
    }
    catch (std::exception& ex)
    {
        std::cout << "\nStandard exception thrown: " << ex.what() << "\n";
        exceptionThrown = true;
    }
    catch (...)
    {
        std::cout << "\nUnexpected exception thrown\n";
        exceptionThrown = true;
    }

    std::cout << "Press enter to complete\n";
    std::getchar();

    if (exceptionThrown)
        return -1;
    else
        return 0;
}