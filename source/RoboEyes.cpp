#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <chrono>
#include <thread>

constexpr int GRID_ROWS = 7;
constexpr int GRID_COLS = 7;   // includes "Level" column

static const char* EMOTIONS[GRID_COLS] = {
    "Level",
    "Anger",
    "Sadness",
    "Happiness",
    "Fatigue",
    "Surprise",
    "Judgment",
};

static const char* INTENSITY[GRID_ROWS] = {
    "Neutral",
    "Mild",
    "Clear",
    "Strong",
    "Strong+",
    "Intense",
    "Intense+"
};

int main()
{
    const std::string imagePath = "../RoboEyes/assets/Eye_Expressions_HD.png";

    cv::Mat atlas = cv::imread(imagePath, cv::IMREAD_GRAYSCALE);
    if (atlas.empty()) {
        std::cerr << "Failed to load: " << imagePath << std::endl;
        return -1;
    }

    const int tileWidth = atlas.cols / GRID_COLS;
    const int tileHeight = atlas.rows / GRID_ROWS;

    cv::Mat display;
    cv::namedWindow("Eye Expression Viewer", cv::WINDOW_AUTOSIZE);

    constexpr int DISPLAY_DELAY_MS = 600; // per expression

    while (true)
    {
        for (int col = 0; col < GRID_COLS; ++col)        
        {
            for (int row = 0; row < GRID_ROWS; ++row)
            {
                cv::Rect roi(
                    col * tileWidth,
                    row * tileHeight,
                    tileWidth,
                    tileHeight
                );

                display = atlas(roi).clone(); // isolated tile

                // Convert to BGR for text overlay
                cv::cvtColor(display, display, cv::COLOR_GRAY2BGR);

                // Text overlay
                std::string label =
                    std::string(INTENSITY[row]) + " / " + EMOTIONS[col];

                cv::putText(
                    display,
                    label,
                    cv::Point(10, tileHeight - 10),
                    cv::FONT_HERSHEY_SIMPLEX,
                    0.5,
                    cv::Scalar(0, 255, 0),
                    1,
                    cv::LINE_AA
                );

                cv::namedWindow("Eye Expression Viewer", cv::WINDOW_NORMAL);

                cv::imshow("Eye Expression Viewer", display);

                int key = cv::waitKey(DISPLAY_DELAY_MS);
                if (key == 27) { // ESC
                    return 0;
                }
            }
        }
    }

    return 0;
}
