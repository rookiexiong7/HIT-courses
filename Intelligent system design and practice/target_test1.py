import os
from typing import List, Tuple

import cv2
import numpy as np
import matplotlib.pyplot as plt


os.chdir(os.path.dirname(__file__))


def calc_background(images: List[cv2.typing.MatLike]) -> cv2.typing.MatLike:
    assert len(images) > 0, "images is empty"
    total = np.zeros(images[0].shape, dtype=np.uint64)
    for image in images:
        total += image
    return (total / len(images)).astype(np.uint8)


def get_target_coordinates(
    image: cv2.typing.MatLike, *, min_area: int = 8, brightness_threshold: int = 30
) -> List[Tuple[int, int, int, int]]:
    """
    Returns the coordinates of the target in the image.
    """
    assert image is not None
    edges = cv2.Canny(image, 100, 200)
    contours, _ = cv2.findContours(edges, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    target_coordinates: List[Tuple[int, int, int, int]] = []

    for contour in contours:
        area = cv2.contourArea(contour)
        if area >= min_area:
            x, y, w, h = cv2.boundingRect(contour)
            roi = image[y : y + h, x : x + w]
            mean_brightness = cv2.mean(roi)[0]
            if mean_brightness >= brightness_threshold:
                target_coordinates.append((x, y, w, h))
    return target_coordinates


def track_targets(images: List[cv2.typing.MatLike], initial_coords: List[Tuple[int, int, int, int]]) -> List[np.ndarray]:
    """
    Tracks targets in a list of images using OpenCV's built-in tracker
    """
    assert images, "Images must be provided"
    assert initial_coords, "Initial coordinates must be provided"
    trackers: List[cv2.Tracker] = []

    for coords in initial_coords:
        tracker = cv2.TrackerCSRT.create()
        tracker.init(images[0], coords)
        trackers.append(tracker)

    old_bboxes: np.ndarray = np.array(initial_coords)
    points: List[np.ndarray] = [(old_bboxes[:, :2] + old_bboxes[:, 2:] / 2).astype(np.int32)]

    # 逐帧追踪
    for i, frame in enumerate(images[1:], start=1):
        new_bboxes = []

        for tracker, old_bbox in zip(trackers, old_bboxes):
            success, bbox = tracker.update(frame)
            if success:
                new_bboxes.append(bbox)
            else:
                print(f"tracker {i} lost")
                # reset tracker
                tracker.init(images[i - 1], old_bbox)
                new_bboxes.append(old_bbox)

        new_bboxes = np.array(new_bboxes)
        points.append((new_bboxes[:, :2] + new_bboxes[:, 2:] / 2).astype(np.int32))
        old_bboxes = new_bboxes
    return points


if __name__ == "__main__":
    # 读取图像
    raw_images = [
        cv2.imread(f"images/t{i:04}.jpg", cv2.IMREAD_GRAYSCALE) for i in range(1, 1024)
    ]

    # 计算背景
    background = calc_background(raw_images[::32])
    images = [cv2.absdiff(img, background) for img in raw_images]

    # 获取初始目标坐标
    initial_coords = get_target_coordinates(images[0], brightness_threshold=25)
    print(f"Initial Target Coordinates: {initial_coords}")

    # 跟踪目标并获取位移
    points = track_targets(images, initial_coords)

    # 打印位移结果
    background = cv2.cvtColor(background, cv2.COLOR_GRAY2BGR)
    # print(points)
    for p1s, p2s in zip(points, points[1:]):
        for p1, p2 in zip(p1s, p2s):
            cv2.line(background, p1, p2, (0, 0, 255), 3)

    plt.imshow(background)
    plt.axis('off')
    plt.title('Tracking Results')
    plt.show()
