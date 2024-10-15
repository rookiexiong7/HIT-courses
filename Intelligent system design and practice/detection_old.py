import os
from typing import List, Tuple

import cv2
import numpy as np
import matplotlib.pyplot as plt

import argparse

def load_images(directory):
    """ 从指定目录加载图像。"""
    image_files = sorted([os.path.join(directory, f) for f in os.listdir(directory) if f.endswith('.png') or f.endswith('.jpg')])
    images = [cv2.imread(file, cv2.IMREAD_GRAYSCALE) for file in image_files]
    return images


def compute_background(images: List[cv2.typing.MatLike]) -> cv2.typing.MatLike:
    """ 通过对图像列表进行平均计算背景图像。"""
    assert len(images) > 0, "images 列表为空"
    
    images_array = np.array(images, dtype=np.uint8)
    background = np.mean(images_array, axis=0).astype(np.uint8)
    
    return background


def find_target_regions(
    image: cv2.typing.MatLike, min_area: int = 8, brightness_threshold: int = 30
) -> List[Tuple[int, int, int, int]]:
    """ 基于面积和亮度识别图像中的潜在目标区域。"""
    assert image is not None
    # 预先计算Canny边缘
    edges = cv2.Canny(image, 100, 200)
    # 找到所有外轮廓
    contours, _ = cv2.findContours(edges, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    target_regions: List[Tuple[int, int, int, int]] = []
    for contour in contours:
        # 计算轮廓面积
        area = cv2.contourArea(contour)
        if area < min_area:
            continue  # 忽略面积过小的轮廓
        # 计算轮廓的边界框
        x, y, w, h = cv2.boundingRect(contour)
        # 利用NumPy批量计算ROI区域的平均亮度
        roi = image[y:y + h, x:x + w]
        mean_brightness = np.mean(roi)
        # 判断亮度是否超过阈值
        if mean_brightness >= brightness_threshold:
            target_regions.append((x, y, w, h))
    
    return target_regions


def track_targets_over_sequence(
    images: List[cv2.typing.MatLike], initial_region: Tuple[int, int, int, int, int]
) -> List[np.ndarray]:
    """ 在图像序列中跟踪目标区域。"""
    assert images and initial_region

    tracker = cv2.TrackerCSRT.create()
    initial_coord = initial_region[1:]
    start_frame = initial_region[0]
    tracker.init(images[start_frame], initial_coord)

    old_bbox: np.ndarray = np.array(initial_coord)
    # 保存每一帧的跟踪中心点
    points: List[np.ndarray] = [(old_bbox[:2] + old_bbox[2:] / 2).astype(np.int32)]

    # 逐帧追踪
    for i, frame in enumerate(images[start_frame+1:], start=1):
        success, bbox = tracker.update(frame)
        if success:
            new_bbox = bbox
        else:
            print(f"tracker lost on frame {i}")
            # reset tracker
            tracker.init(images[i - 1], old_bbox)
            new_bbox = old_bbox

        points.append((np.array(new_bbox[:2]) + np.array(new_bbox[2:]) / 2).astype(np.int32))
        old_bbox = np.array(new_bbox)

    return np.array(points)


def first_nframes_detection(images, n_frames, min_size, brightness_threshold):
    ''' 根据前n帧图像检测出的目标是否为直线运动，排除掉非直线运动的目标 '''
    
    # background = compute_background(images[:n_frames])
    # processed_images = [cv2.absdiff(img, background) for img in images]
    initial_target_regions = [find_target_regions(images[i], min_area=min_size, brightness_threshold=brightness_threshold) for i in range(n_frames)]
    detected_targets = []

    initial_detected_targets = {f'0,{x},{y},{w},{h}':[(0,x,y,w,h)] for x,y,w,h in initial_target_regions[0]}
    # 初始化一个以第一帧检测出的目标区域为基准的目标区域字典

    for i in range(1, len(initial_target_regions)):
        for j in range(len(initial_target_regions[i])):
            x1, y1, w1, h1 = initial_target_regions[i][j]
            target_area = images[i][y1:y1+h1, x1:x1+w1]
            flag = False

            for key in list(initial_detected_targets.keys()):
                index, x0, y0, w0, h0 = initial_detected_targets[key][-1]
                previous_target_area = images[index][y0:y0+h0, x0:x0+w0]

                d = abs(i-index) * 4
                distance = np.sqrt((x1-x0)**2 + (y1-y0)**2)
                # 目标在图像上的运动速度1—4像素/帧，亮度变化范围+/-15%
                if distance <= d and np.mean(target_area) >= 0.85*np.mean(previous_target_area):
                    initial_detected_targets[key].append((i, x1, y1, w1, h1))
                    flag = True
                    break

            if not flag:
                initial_detected_targets[f'{i},{x1},{y1},{w1},{h1}'] = [(i, x1, y1, w1, h1)]

    for key in initial_detected_targets.keys():
        point_list = initial_detected_targets[key]
        if len(point_list) > 3:
            points = np.array([(x,y) for _,x,y,_,_ in point_list])
            x = points[:,0]
            y = points[:,1]
            z = np.polyfit(x, y, 1)
            # 计算所有点到直线的距离的均值，小于阈值则加入
            if np.mean(np.abs(y - (z[0]*x + z[1]))) <= 2:
                detected_targets.append(key)

    return detected_targets


def visualize_tracking_results(background: cv2.typing.MatLike, tracking_points: np.ndarray):
    background = cv2.cvtColor(background, cv2.COLOR_GRAY2BGR)

    # 遍历每一帧的跟踪点，并在背景图像上绘制线条
    for i in range(1, tracking_points.shape[0]):
        x0, y0 = tracking_points[i - 1]
        x1, y1 = tracking_points[i]
        cv2.line(background, (x0, y0), (x1, y1), (0, 255, 0), 2)


    # 显示最终的跟踪结果图像
    plt.imshow(background)
    plt.axis('off')
    plt.title('Tracking Results')
    plt.show()


def main(args):
    raw_images = load_images(args.image_dir)
    background = compute_background(raw_images[::32])
    processed_images = [cv2.absdiff(img, background) for img in raw_images]

    detected_targets = first_nframes_detection(processed_images, args.first_frames, args.min_size, args.brightness_threshold)
    detected_targets = [tuple(map(int, target.split(','))) for target in detected_targets]
    print(f"前{args.first_frames}帧图像检测出的目标: {detected_targets}")
    
    for init_target in detected_targets:
        tracked_points = track_targets_over_sequence(processed_images, init_target)
        visualize_tracking_results(background, tracked_points)

# TODO 实时计算，并且实时移除不可靠的目标区域
# TODO 噪声分析
# TODO 可视化优化，可以以视频形式？

if __name__ == "__main__":
    # TODO 实时计算，并且实时移除不可靠的目标区域
    # TODO 噪声分析
    parser = argparse.ArgumentParser()
    parser.add_argument('--image_dir', type=str, default='images', help='Path to the images directory')
    parser.add_argument('--first_frames', type=int, default=100, help='Number of frames to use for detection')
    parser.add_argument('--min_size', type=int, default=8, help='Minimum size of detected target regions')
    parser.add_argument('--brightness_threshold', type=int, default=30, help='Brightness threshold for target regions')
    args = parser.parse_args()

    main(args)