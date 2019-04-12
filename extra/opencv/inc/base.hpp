#ifndef __OCV_BASE_HPP__
#define __OCV_BASE_HPP__

#include <opencv2/core/core.hpp>

namespace u {
    namespace ocv {

        bool point_compare_func(const cv::Point &p1, const cv::Point &p2) {
            bool ans = true;
            if (p1.x > p2.x) {
                ans = false;
            } else if (p1.x == p2.x) {
                if (p1.y >= p2.y) {
                    ans = false;
                }
            }
            return ans;
        }

        void concat(cv::InputArray &_src1, cv::InputArray &_src2,
                    cv::OutputArray &_dst, int interval=0,
                    cv::Scalar value=cv::Scalar(0), bool horizon=true) {
            cv::Mat src1 = _src1.getMat();
            cv::Mat src2 = _src2.getMat();
            CV_Assert(src1.type() == src2.type());
            if (horizon) {
                CV_Assert(src1.rows == src2.rows);
                cv::Mat extend;
                if (interval > 0) {
                    cv::Mat inter(src1.rows, interval, src1.type());
                    inter.setTo(value);
                    cv::hconcat(src1, inter, extend);
                } else {
                    extend = src1.clone();
                }
                cv::hconcat(extend, src2, _dst);
            } else {
                CV_Assert(src1.cols == src2.cols);
                cv::Mat extend;
                if (interval > 0) {
                    cv::Mat inter(interval, src1.cols, src1.type());
                    inter.setTo(value);
                    cv::vconcat(src1, inter, extend);
                } else {
                    extend = src1.clone();
                }
                cv::vconcat(extend, src2, _dst);
            }
        }

        cv::Mat concat(cv::InputArray &_src1, cv::InputArray &_src2,
                       int interval=0, cv::Scalar value=cv::Scalar(0),
                       bool horizon=true) {
            cv::Mat ans;
            concat(_src1, _src2, ans, interval, value, horizon);
            return ans;
        }
    }
}

#endif
