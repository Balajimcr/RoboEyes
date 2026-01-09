#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

constexpr int GRID_ROWS = 7;   // data rows only
constexpr int GRID_COLS = 7;   // data cols only
constexpr int PLAYBACK_DELAY_MS = 600;

struct ExpressionLabels {
    std::vector<std::vector<std::string>> table; // includes headers
};

// ------------------------------------------------------------
// Load TAB-separated CSV (with row + column headers)
// ------------------------------------------------------------
static bool loadExpressionCSV(
    const std::string& path,
    ExpressionLabels& out)
{
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open CSV: " << path << "\n";
        return false;
    }

    out.table.clear();
    std::string line;

    while (std::getline(file, line)) {
        std::vector<std::string> row;
        std::string cell;

        for (char c : line) {
            if (c == ',') {
                row.push_back(cell);
                cell.clear();
            }
            else {
                cell += c;
            }
        }
        row.push_back(cell);
        out.table.push_back(row);
    }

    return true;
}

int main()
{
    const std::string imagePath = "assets/Eye_Expressions_HD.png";
    const std::string csvPath = "assets/Expressions_Label.csv";

    // ------------------------------------------------------------
    // Load image atlas
    // ------------------------------------------------------------
    cv::Mat atlas = cv::imread(imagePath, cv::IMREAD_GRAYSCALE);
    if (atlas.empty()) {
        std::cerr << "Failed to load atlas image\n";
        return -1;
    }

    // ------------------------------------------------------------
    // Load CSV
    // ------------------------------------------------------------
    ExpressionLabels labels;
    if (!loadExpressionCSV(csvPath, labels)) {
        std::cerr << "Failed to load CSV\n";
        return -1;
    }

    // Expect 8x8 CSV (1 header row + 7 data rows)
    if (labels.table.size() != GRID_ROWS + 1 ||
        labels.table[0].size() != GRID_COLS + 1) {
        std::cerr << "CSV must be 8x8 (with headers)\n";
    }

    // ------------------------------------------------------------
    // Tile geometry
    // ------------------------------------------------------------
    const int tileW = atlas.cols / GRID_COLS;
    const int tileH = atlas.rows / GRID_ROWS;

    int row = 0;   // [0..6] data row
    int col = 0;   // [0..6] data column
    bool autoplay = true;

    cv::namedWindow("RoboEyes", cv::WINDOW_AUTOSIZE);

    // ------------------------------------------------------------
    // Main loop
    // ------------------------------------------------------------
    while (true)
    {
        cv::Rect roi(
            col * tileW,
            row * tileH,
            tileW,
            tileH
        );

        cv::Mat tile;
        atlas(roi).copyTo(tile);
        cv::cvtColor(tile, tile, cv::COLOR_GRAY2BGR);

        // --------------------------------------------------------
        // Correct CSV access (skip headers)
        // --------------------------------------------------------
        const std::string& intensity =
            labels.table[row + 1][0];          // row header

        const std::string& emotion =
            labels.table[0][col + 1];          // column header

        const std::string& state =
            labels.table[row + 1][col + 1];    // data cell

        std::string text = state;

        cv::putText(
            tile,
            text,
            cv::Point(10, tileH - 10),
            cv::FONT_HERSHEY_SIMPLEX,
            0.45,
            cv::Scalar(0, 255, 0),
            1,
            cv::LINE_AA
        );

        cv::imshow("RoboEyes", tile);

        int key = cv::waitKey(autoplay ? PLAYBACK_DELAY_MS : 0);
        bool manualInput = false;

        switch (key)
        {
        case 27: // ESC
            return 0;

        case ' ':
            autoplay = !autoplay;
            break;

        case 81: case 'a': // LEFT
            col = (col > 0) ? col - 1 : GRID_COLS - 1;
            manualInput = true;
            break;

        case 83: case 'd': // RIGHT
            col = (col + 1) % GRID_COLS;
            manualInput = true;
            break;

        case 82: case 'w': // UP
            row = (row > 0) ? row - 1 : GRID_ROWS - 1;
            manualInput = true;
            break;

        case 84: case 's': // DOWN
            row = (row + 1) % GRID_ROWS;
            manualInput = true;
            break;

        default:
            break;
        }

        // Auto-play advance
        if (autoplay && !manualInput) {
            col++;
            if (col >= GRID_COLS) {
                col = 0;
                row = (row + 1) % GRID_ROWS;
            }
        }
    }
}
