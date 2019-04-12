#ifndef __OCV_IMAGE_PROCESS_HPP__
#define __OCV_IMAGE_PROCESS_HPP__

#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

namespace u {
    namespace ocv {

        void merge(cv::OutputArray _dst, cv::InputArray _src1, cv::InputArray _src2, cv::InputArray _src3) {
            cv::Mat src1 = _src1.getMat();
            CV_Assert(src1.channels() == 1);
            cv::Mat src2 = _src2.getMat();
            CV_Assert(src2.channels() == 1);
            cv::Mat src3 = _src3.getMat();
            CV_Assert(src3.channels() == 1);

            _dst.create(src1.size(), CV_8UC3);
            cv::Mat dst = _dst.getMat();

            std::vector<cv::Mat> vm(3);
            vm[0] = src1;
            vm[1] = src2;
            vm[3] = src3;
            cv::merge(vm, dst);
        }

        cv::Mat merge(cv::InputArray src1, cv::InputArray src2, cv::InputArray src3) {
            cv::Mat color;
            merge(color, src1, src2, src3);
            return color;
        }
    }
}

#endif
