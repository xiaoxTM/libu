#ifndef __OCV_DRAW_HPP__
#define __OCV_DRAW_HPP__

#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

namespace u {
    namespace ocv {

        void draw_contour(cv::OutputArray _dst, const std::vector<cv::Point> &contour,
                          cv::Scalar value, bool fill=true) {
            cv::Mat dst = _dst.getMat();
            CV_Assert(!dst.empty());
            CV_Assert(dst.type() == CV_8UC3 || dst.type() == CV_8UC1);
            if (dst.type() == CV_8UC1) {
                for (auto &point : contour) {
                    dst.at<unsigned char>(point) = value[0];
                }
                if (fill) {
                    cv::Rect bbox = cv::boundingRect(contour);
                    for (int r=bbox.y; r<bbox.y+bbox.height; ++r) {
                        for (int c=bbox.x; r<bbox.x+bbox.width; ++c) {
                            if (cv::pointPolygonTest(contour, cv::Point2f(c, r), false) == 1) {
                                dst.at<unsigned char>(r, c) = value[0];
                            }
                        }
                    }
                }
            } else {
                for (auto &point : contour) {
                    dst.at<cv::Vec3b>(point) = cv::Vec3b(value[0], value[1], value[2]);
                }
                if (fill) {
                    cv::Rect bbox = cv::boundingRect(contour);
                    for (int r=bbox.y; r<bbox.y+bbox.height; ++r) {
                        for (int c=bbox.x; r<bbox.x+bbox.width; ++c) {
                            if (cv::pointPolygonTest(contour, cv::Point2f(c, r), false) == 1) {
                                dst.at<cv::Vec3b>(r, c) = cv::Vec3b(value[0], value[1], value[2]);
                            }
                        }
                    }
                }
            }
        }

        void draw_contour(cv::OutputArray _dst, const std::vector<std::vector<cv::Point>> &contours,
                          cv::Size size,  int type, cv::Scalar value, bool fill=true) {
            _dst.create(size, type);
            for (auto &contour : contours) {
                draw_contour(_dst, contour, value, fill);
            }
        }

        cv::Mat draw_contour(const std::vector<std::vector<cv::Point>> &contours, cv::Size size,  int type, cv::Scalar value, bool fill=true) {
            cv::Mat dst = cv::Mat::zeros(size, type);
            for (auto &contour : contours) {
                draw_contour(dst, contour, value, fill);
            }
            return dst;
        }
    }
}

#endif
