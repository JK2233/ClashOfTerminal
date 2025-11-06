#pragma execution_character_set("utf-8")

//includes
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <uchar.h>
#include <vector>
#include <string>
#include <iostream>
#include <ctime>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <fstream>


namespace render
{
/// <summary>
/// VideoStreamMenager is a class that can hold and operate on video files
/// </summary>
class VideoStreamMenager
{

public:
    enum FileFormat : uint8_t
    {
        //a folder with a lot of BMP files
        e_BMP,
        //a "smart" fileformat for storing entire videos in a single file (or so I hope)
        e_DzadzV,
    };

    VideoStreamMenager(std::string _path, uint16_t _amountOfFrames, FileFormat _fileFormat)
    {
        if (_fileFormat == e_BMP) {
            for (uint_fast16_t currentFrameIDX = 1; currentFrameIDX <= _amountOfFrames; currentFrameIDX++)
            {
                //load the frame
                std::ifstream file;

                Frame currentFrame = Frame();

                isCompresed = false;

                std::string filePath = _path;
                uint8_t amountOfNumbers = 0;
                int_fast32_t firstHashtagIDX = 0;
                for (int_fast16_t i = 0; i < filePath.size(); i++)
                {
                    if (filePath[i] == '#')
                    {
                        firstHashtagIDX = i;
                        break;
                    }
                }
                for (int_fast16_t i = 0; i < filePath.size(); i++)
                {
                    if (filePath[i] == '#')
                    {
                        amountOfNumbers++;
                    }
                }
                if (amountOfNumbers != 0) {
                    std::string fileNameNumber = std::to_string(currentFrameIDX);
                    while (fileNameNumber.size() < amountOfNumbers) {
                        fileNameNumber = "0" + fileNameNumber;
                    }
                    filePath.replace(firstHashtagIDX, amountOfNumbers, fileNameNumber);
                }
                file.open(filePath, std::ios::binary | std::ios::in);
                if (!file) {
                    ReportError("Failed to open the file" + filePath, true);
                    return;
                }
                const int_fast16_t sizeOfFileHeader = 14;
                const int_fast16_t sizeOfHeaderInformation = 40;

                //read the header of the file and store it in fileHeader
                uint8_t fileHeader[sizeOfFileHeader];
                file.read(reinterpret_cast<char*>(fileHeader), sizeOfFileHeader);

                //do the same for the HeaderInformation
                uint8_t headerInformation[sizeOfHeaderInformation];
                file.read(reinterpret_cast<char*>(headerInformation), sizeOfHeaderInformation);

                //as per specification the 2. to 5. byte of BMP header contains the size of the file
                int32_t fileSize = fileHeader[2] + (fileHeader[3] << 8) + (fileHeader[4] << 16) + (fileHeader[5] << 24);

                //geting the height and width of the image
                currentFrame.sizeX = headerInformation[4] + (headerInformation[5] << 8) + (headerInformation[6] << 16) + (headerInformation[7] << 24);
                currentFrame.sizeY = headerInformation[8] + (headerInformation[9] << 8) + (headerInformation[10] << 16) + (headerInformation[11] << 24);

                //Nope there ain't no reson to go above 8k
                if (currentFrame.sizeX > 8192 || currentFrame.sizeY > 8192)
                {
                    ReportError("It apears that somebody has tried to load a video of rediculus size... plise don't do that", true, true);
                }

                currentFrame.color = new uint8_t[currentFrame.sizeX * currentFrame.sizeY];

                //WHY? why did somebody thought that this padding is a good idea for an image file format... BMP is already large enough
                const int padding = ((4 - (currentFrame.sizeX * 3) % 4) % 4);

                for (int_fast32_t Y = 0; Y < currentFrame.sizeY; ++Y) {
                    for (int_fast32_t X = 0; X < currentFrame.sizeX; ++X) {
                        uint8_t color[4];
                        file.read(reinterpret_cast<char*>(color), 4);

                        currentFrame.color[Y * currentFrame.sizeX + X] = color[1];
                    }

                    file.ignore(padding);

                }
                file.close();
                frames.push_back(currentFrame);
            }
        }
        if (_fileFormat == e_DzadzV) {
            std::ifstream file;
            file.open(_path, std::ios::in | std::ios::binary);
            if (file.is_open()) {
                const int_fast16_t sizeOfFileHeader = 5;

                //read the header of the file and store it in fileHeader
                uint8_t fileHeader[sizeOfFileHeader];
                file.read(reinterpret_cast<char*>(fileHeader), sizeOfFileHeader);

                uint16_t videoSizeX = fileHeader[1] * 256 + fileHeader[2];
                uint16_t videoSizeY = fileHeader[3] * 256 + fileHeader[4];

                const int32_t frameSize = videoSizeX * videoSizeY;

                //original uncompressed dzadzV fileformat
                if (fileHeader[0] == 0) {
                    for (int32_t i = 0 ; i < _amountOfFrames ; ++i) {
                        Frame tmpFrame;
                        tmpFrame.color = new uint8_t[frameSize];
                        tmpFrame.sizeX = videoSizeX;
                        tmpFrame.sizeY = videoSizeY;
                        file.read(reinterpret_cast<char*>(tmpFrame.color), frameSize);
                        frames.push_back(tmpFrame);
                    }
                }
                if (fileHeader[0] == 1) {
                    isCompresed = true;
                    for (int32_t i = 0 ; i < _amountOfFrames ; ++i) {
                        Frame tmpFrame;
                        tmpFrame.color = new uint8_t[frameSize];
                        tmpFrame.sizeX = videoSizeX;
                        tmpFrame.sizeY = videoSizeY;
                        file.read(reinterpret_cast<char*>(tmpFrame.color), frameSize);
                        frames.push_back(tmpFrame);
                    }
                }

            }
            else {
                ReportError("cen't find file " + _path + " plise fix", false);
            }
        }

    }

    ~VideoStreamMenager()
    {
    }

    int32_t getColorAtFrame(uint16_t Y, uint16_t X) {
        if (frameNO >= frames.size())
        {
            return -2;
        }
        if (X > frames[frameNO].sizeX || Y > frames[frameNO].sizeY || X < 0 || Y < 0)
        {
            //error code
            return -1;
        }
        return frames[frameNO].color[(Y * frames[frameNO].sizeX) + X];
    }

    void exportAsDzadzV(std::string _filePath, bool _useCompresion) {
        std::vector<uint8_t> fileBuffer;
        //DzadzV version metadata
        fileBuffer.push_back(1);

        fileBuffer.push_back(frames[0].sizeX / 256);
        fileBuffer.push_back(frames[0].sizeX & 0b0000000011111111);
        fileBuffer.push_back(frames[0].sizeY / 256);
        fileBuffer.push_back(frames[0].sizeY & 0b0000000011111111);

        const int32_t frameSize = frames[0].sizeY * frames[0].sizeX;

        uint8_t frameSimulatiry = 20;

        for (uint16_t i = 0; i < frames.size(); ++i)
        {
            if (i == 0) {
                frameSimulatiry = 0;
                continue;
            }
            for (size_t j = 0; j < frameSize; ++j)
            {
                if (frames[i].color[j] == frames[i-1].color[j]) {
                    frameSimulatiry--;
                    if (frameSimulatiry == 0) {
                        break;
                    }
                } 
            }
            if (frameSimulatiry != 0) {
                fileBuffer.push_back(0);
                for (size_t j = 0; j < frameSize; ++j)
                {
                    fileBuffer.push_back(frames[i].color[j]);
                }
            }
            else {
                fileBuffer.push_back(1);
            }
        }
        std::ofstream file;
        file.open(_filePath, std::ios::out | std::ios::binary);
        {
            char* tmp = new char[fileBuffer.size()];
            for (size_t i = 0; i < fileBuffer.size(); ++i)
            {
                tmp[i] = fileBuffer[i];
            }
            file.write(tmp, fileBuffer.size());
        }
        file.close();
    }

    uint16_t frameNO = 0;
private:
    bool isCompresed;

    struct Frame
    {
        int32_t sizeX;
        int32_t sizeY;
        uint8_t* color;
    };

    FileFormat format;

    std::vector<Frame> frames;
};


}