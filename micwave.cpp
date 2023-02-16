#include <iostream>
#include "portaudio.h"
#include <cmath>
#define SAMPLE_RATE (44100)
#define FRAMES_PER_BUFFER (1024)
#define NUM_STARS (20)
#define THRESHOLD_FACTOR (4)

int main() {
    PaError err;
    err = Pa_Initialize();
    if (err != paNoError) {
        std::cout << "Error: Failed to initialize PortAudio" << std::endl;
        return 1;
    }

    PaStreamParameters inputParameters;
    inputParameters.device = Pa_GetDefaultInputDevice();
    inputParameters.channelCount = 1;
    inputParameters.sampleFormat = paFloat32;
    inputParameters.suggestedLatency = Pa_GetDeviceInfo(inputParameters.device)->defaultLowInputLatency;
    inputParameters.hostApiSpecificStreamInfo = NULL;

    PaStream* stream;
    err = Pa_OpenStream(
        &stream,
        &inputParameters,
        NULL,
        SAMPLE_RATE,
        FRAMES_PER_BUFFER,
        paClipOff,
        NULL,
        NULL
    );
    if (err != paNoError) {
        std::cout << "Error: Failed to open audio stream" << std::endl;
        return 1;
    }

    err = Pa_StartStream(stream);
    if (err != paNoError) {
        std::cout << "Error: Failed to start audio stream" << std::endl;
        return 1;
    }

    float* buffer = new float[FRAMES_PER_BUFFER];
    while (true) {
        err = Pa_ReadStream(stream, buffer, FRAMES_PER_BUFFER);
        if (err != paNoError) {
            std::cout << "Error: Failed to read audio stream" << std::endl;
            break;
        }

        float rms = 0;
        for (int j = 0; j < FRAMES_PER_BUFFER; j++) {
            rms += buffer[j] * buffer[j];
        }
        rms = sqrt(rms / FRAMES_PER_BUFFER);

        int num_stars = (int)(rms * NUM_STARS * THRESHOLD_FACTOR);
        for (int j = 0; j < num_stars; j++) {
            std::cout << "*";
        }
        std::cout << std::endl;
    }

    err = Pa_StopStream(stream);
    if (err != paNoError) {
        std::cout << "Error: Failed to stop audio stream" << std::endl;
        return 1;
    }

    err = Pa_CloseStream(stream);
    if (err != paNoError) {
        std::cout << "Error: Failed to close audio stream" << std::endl;
        return 1;
    }

    Pa_Terminate();

    delete[] buffer;

    return 0;
}
