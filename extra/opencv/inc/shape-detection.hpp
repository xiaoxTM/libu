#ifndef __OCV_SHAPE_DETECTION_HPP__
#define __OCV_SHAPE_DETECTION_HPP__

#include <opencv2/imgproc/imgproc.hpp>

namespace u {

    namespace ocv {

        enum shape_t {
            TRIANGLE,
            SQUARE,
            RECTANGLE,
            PENTAGON,
            CIRCLE
        };

        int shape_detection(const std::vector<cv::Point> &contour, double scale=0.01, bool closed=true) {
            double peri = cv::arcLength(contour, closed);
            cv::Mat approx;
            cv::approxPolyDP(contour, approx, scale * peri, closed); // 0.01 ~ 0.05

            int shape;

            if (approx.rows == 3) {
                shape = TRIANGLE; // triangle
            } else if (approx.rows == 4) {
                cv::Rect r = cv::boundingRect(approx);

                double ar = 1.0 * r.width / r.height;

                if (ar >= 0.95 && ar <= 1.05) {
                    shape = SQUARE; // square
                } else {
                    shape = RECTANGLE; // rectangle
                }
            } else if (approx.rows == 5) {
                shape = PENTAGON; // pentagon
            } else {
                shape = approx.rows;
            }

            return shape;
        }

    }
}

#endif
