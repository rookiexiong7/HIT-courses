import os
from typing import List, Tuple
import cv2
import numpy as np
import argparse


def load_images(directory) -> np.ndarray:
    """
    从指定目录加载图像。
    @param directory: 图像目录
    """
    print(f"正在从 {directory} 目录加载图像...")
    image_files = sorted(
        [os.path.join(directory, f) for f in os.listdir(directory) if f.endswith('.png') or f.endswith('.jpg')])
    images = np.array([cv2.imread(file, cv2.IMREAD_GRAYSCALE) for file in image_files])
    return images


def compute_background(images: List[cv2.typing.MatLike]) -> cv2.typing.MatLike:
    """
    通过对图像列表进行平均计算背景图像。
    @param images: 图像列表
    """
    assert len(images) > 0, "images 列表为空"
    images_array = np.array(images, dtype=np.uint8)
    background = np.mean(images_array, axis=0).astype(np.uint8)

    return background


def find_target_regions(
        image: cv2.typing.MatLike, min_area: int = 8, brightness_threshold: int = 30
) -> List[Tuple[int, int, int, int]]:
    """
    基于面积和亮度识别图像中的潜在目标区域，得到面积和亮度均大于阈值的区域列表。
    @param image: 输入图像
    @param min_area: 最小面积阈值
    @param brightness_threshold: 亮度阈值
    """
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


def first_nframes_detection(images, n_frames, min_size, brightness_threshold):
    """ 根据前n帧图像检测出的目标是否为直线运动，排除掉非直线运动的目标 """
    print("正在检测追踪目标...")
    background = compute_background(images[:n_frames])
    processed_images = [cv2.absdiff(img, background) for img in images]
    initial_target_regions = [
        find_target_regions(processed_images[i], min_area=min_size, brightness_threshold=brightness_threshold) for i in
        range(n_frames)]  # 首先检测前n张图片中，所有ROI区域
    detected_targets = []

    initial_detected_targets = {f'0,{x},{y},{w},{h}': [(0, x, y, w, h)] for x, y, w, h in initial_target_regions[0]}
    # 初始化一个以第一帧检测出的目标区域为基准的目标区域字典

    for i in range(1, len(initial_target_regions)):
        for j in range(len(initial_target_regions[i])):
            x1, y1, w1, h1 = initial_target_regions[i][j]
            target_area = images[i][y1:y1 + h1, x1:x1 + w1]
            flag = False

            for key in list(initial_detected_targets.keys()):
                index, x0, y0, w0, h0 = initial_detected_targets[key][-1]
                previous_target_area = images[index][y0:y0 + h0, x0:x0 + w0]

                d = abs(i - index) * 4
                distance = np.sqrt((x1 - x0) ** 2 + (y1 - y0) ** 2)
                # 目标在图像上的运动速度1—4像素/帧，亮度变化范围+/-15%
                if distance <= d and np.mean(target_area) >= 0.85 * np.mean(previous_target_area):
                    initial_detected_targets[key].append((i, x1, y1, w1, h1))
                    flag = True
                    break

            if not flag:
                initial_detected_targets[f'{i},{x1},{y1},{w1},{h1}'] = [(i, x1, y1, w1, h1)]

    for key in initial_detected_targets.keys():
        point_list = initial_detected_targets[key]
        if len(point_list) > 3:
            points = np.array([(x, y) for _, x, y, _, _ in point_list])
            x = points[:, 0]
            y = points[:, 1]
            z = np.polyfit(x, y, 1)
            # 计算所有点到直线的距离的均值，小于阈值则加入
            if np.mean(np.abs(y - (z[0] * x + z[1]))) <= 2:
                detected_targets.append(key)

    return detected_targets


def track_and_visualize(
        images: np.ndarray, initial_region: Tuple[int, int, int, int, int], video_path: str,
        draw_trajectory: bool = False, index: int = 0
) -> np.ndarray:
    """ 在图像序列中跟踪目标区域，并实时显示跟踪结果，同时保存为视频。"""
    print("正在追踪目标...")

    # 定义视频的分辨率和帧率
    frame_width = int(images[0].shape[1])
    frame_height = int(images[0].shape[0])
    fps = 25.0

    # 定义视频编解码器和创建 VideoWriter 对象
    fourcc = cv2.VideoWriter_fourcc(*'XVID')  # .avi
    # fourcc = cv2.VideoWriter_fourcc(*'X264')  # .mp4
    video_path = f'{video_path}_{index}.avi'
    if not os.path.exists(os.path.dirname(video_path)):
        os.makedirs(os.path.dirname(video_path))
    out = cv2.VideoWriter(video_path, fourcc, fps, (frame_width, frame_height))

    tracker = cv2.TrackerCSRT.create()
    initial_coord = initial_region[1:]
    start_frame = initial_region[0]
    tracker.init(images[start_frame], initial_coord)

    points: List[np.ndarray] = []

    cv2.namedWindow('Tracking Results', cv2.WINDOW_NORMAL)  # 创建 OpenCV 窗口

    # 逐帧追踪
    for i, frame in enumerate(images[start_frame + 1:], start=1):
        success, bbox = tracker.update(frame)
        if success:
            new_point = (np.array(bbox[:2]) + np.array(bbox[2:]) / 2).astype(np.int32)
            points.append(new_point)
        else:
            print(f"tracker lost on frame {i}")
            # 如果跟踪失败，使用上一个点作为当前点
            new_point = points[-1] if points else (
                    np.array(initial_coord[:2]) + np.array(initial_coord[2:]) / 2).astype(np.int32)
            points.append(new_point)
        background = cv2.cvtColor(images[i + start_frame], cv2.COLOR_GRAY2BGR)  # 更新背景图像

        # 绘制轨迹
        if draw_trajectory and points:
            for j in range(1, len(points)):
                cv2.line(background, tuple(points[j - 1]), tuple(points[j]), (50, 255, 170), 2)

        x, y, w, h = bbox
        # 调整因子，控制框的缩放大小
        adjustment_x = 4
        adjustment_y = 4

        # 计算坐标
        top_left_x = int(x - w * (adjustment_x - 1) / 2)
        top_left_y = int(y - h * (adjustment_y - 1) / 2)
        bottom_right_x = int(x + w * (adjustment_x - 1) / 2)
        bottom_right_y = int(y + h * (adjustment_y - 1) / 2)

        # 确保坐标在图像边界内
        top_left_x = max(0, min(top_left_x, background.shape[1] - 1))
        top_left_y = max(0, min(top_left_y, background.shape[0] - 1))
        bottom_right_x = max(0, min(bottom_right_x, background.shape[1] - 1))
        bottom_right_y = max(0, min(bottom_right_y, background.shape[0] - 1))

        # 绘制矩形框
        cv2.rectangle(background, (top_left_x, top_left_y), (bottom_right_x, bottom_right_y), (0, 255, 0), 2)

        # 显示当前的跟踪结果
        cv2.imshow('Tracking Results', background)
        out.write(background)  # 保存到视频文件

        # 按 'q' 退出
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

    print("追踪结束")
    # cv2.waitKey(0)
    cv2.destroyAllWindows()
    out.release()  # 释放视频写入对象
    return np.array(points)


def add_noise(images: np.ndarray, noise_type: str, noise_param_1: float, noise_param_2: float, batch_size=50):
    print("正在添加噪声……")
    assert noise_type in ['gaussian', 'salt_and_pepper']
    n = images.shape[0]
    x = images.shape[1]
    y = images.shape[2]
    if noise_type == 'gaussian':
        mean, sigma = noise_param_1, noise_param_2
        total_batch = np.ceil(n / batch_size).astype(np.uint)
        for i in range(total_batch):
            print(f'进度：{i + 1}/{total_batch}')
            start = i * batch_size
            end = min(n, (i + 1) * batch_size)
            noise: np.ndarray = np.random.normal(loc=mean, scale=sigma, size=(end - start, x, y))
            batch = images[start: end].astype(np.float32) / 255.0
            batch = batch + noise
            batch = np.clip(batch, 0, 1)
            batch = (batch * 255.0).astype(np.uint8)
            images[start: end] = batch
    else:
        salt, pepper = int(noise_param_1 * x * y), int(noise_param_2 * x * y)
        for i in range(n):
            salt_x_coords = np.random.randint(0, x, salt)
            salt_y_coords = np.random.randint(0, y, salt)
            images[i][salt_x_coords, salt_y_coords] = 255
            pepper_x_coords = np.random.randint(0, x, pepper)
            pepper_y_coords = np.random.randint(0, y, pepper)
            images[i][pepper_x_coords, pepper_y_coords] = 0
    return images


def draw_trajectory_on_image(image, points, image_output_path, index=0):
    image = cv2.cvtColor(image, cv2.COLOR_GRAY2BGR)
    for j in range(1, len(points)):
        cv2.line(image, tuple(points[j - 1]), tuple(points[j]), (0, 255, 0), 2)
    # 保存轨迹图像
    image_output_path = f'{image_output_path}_{index}.jpg'
    if not os.path.exists(os.path.dirname(image_output_path)):
        os.makedirs(os.path.dirname(image_output_path))
    cv2.imwrite(image_output_path, image)


def main(args):
    raw_images = load_images(args.image_dir)  # 读取全部灰度图像
    # processed_images = [cv2.absdiff(img, background) for img in raw_images]     # 将每张图片减去背景图，得到降噪后的图像

    # 添加噪声
    if args.noise:
        noised_images = add_noise(raw_images,
                                  args.noise_type,
                                  args.noise_param_1,
                                  args.noise_param_2,
                                  args.noise_batch_size)
    else:
        noised_images = raw_images

    detected_targets = first_nframes_detection(noised_images, args.first_frames, args.min_size,
                                               args.brightness_threshold)
    detected_targets = [tuple(map(int, target.split(','))) for target in detected_targets]
    # del processed_images
    print(f"前{args.first_frames}帧图像检测出的目标: {detected_targets}")

    # 这里也可以添加噪声

    for index, init_target in enumerate(detected_targets):
        init_target: tuple[int, int, int, int, int]
        points = track_and_visualize(noised_images, init_target, args.video_path, draw_trajectory=args.draw_trajectory, index=index)
        if args.output_image_path is not None:
            background = compute_background(noised_images[::32])  # 以32为采样步长，计算背景图
            draw_trajectory_on_image(background, points, args.output_image_path, index)



if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument('--image_dir', type=str, default='images', help='Path to the images directory')
    parser.add_argument('--first_frames', type=int, default=100, help='Number of frames to use for detection')
    parser.add_argument('--min_size', type=int, default=8, help='Minimum size of detected target regions')
    parser.add_argument('--brightness_threshold', type=int, default=30, help='Brightness threshold for target regions')
    parser.add_argument('--video_path', type=str, default='results_videos/test_noise', help='Path to save result video')
    parser.add_argument('--draw_trajectory', type=bool, default=True, help='Draw trajectory of the target')
    parser.add_argument('--output_image_path', type=str, default='results_images/test_noise', help='Path to save trajectory image')
    # 噪声控制
    parser.add_argument('--noise', type=bool, default=True, help='Add noise')
    parser.add_argument('--noise_type', type=str, default='salt_and_pepper', help='Noise type', choices=['gaussian', 'salt_and_pepper'])
    parser.add_argument('--noise_param_1', type=float, default=0.01,
                        help='If use gaussian, this is mean. If use salt and pepper, this means salt prob.')
    parser.add_argument('--noise_param_2', type=float, default=0.0,
                        help='If use gaussian, this is sigma. If use salt and pepper, this means pepper prob.')
    parser.add_argument('--noise_batch_size', type=int, default=50, help='Batch size when adding noise on images.')
    args = parser.parse_args()
    main(args)
