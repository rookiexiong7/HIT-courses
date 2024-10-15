@REM 测试噪声水平的影响
chcp 65001 >nul
@echo off
setlocal EnableDelayedExpansion

:: 设置环境
set ENV_NAME=xiong
set SCRIPT_PATH=detection_new.py
set IMAGE_DIR=images

:: 针对guassian声类型，分别设置不同的噪声参数
call activate %ENV_NAME%
:: 遍历不同的噪声参数
for %%a in (gaussian) do (
    for %%b in (0.01 0.02 0.03 0.04 0.06 0.07) do (
            :: 激活环境并运行脚本
            set OUTPUT_IMAGE=results_images/%%a_%%b.jpg
            set VIDEO_PATH=results_videos/%%a_%%b.avi
            echo 在 %%a 噪声类型下，噪声参数为 %%b
            python %SCRIPT_PATH% --image_dir %IMAGE_DIR% --video_path results_videos/%%a_%%b.avi --output_image_path results_images/%%a_%%b.jpg --noise True --noise_type %%a --noise_param_1 0 --noise_param_2 %%b
    )
)

@REM for %%a in (salt_and_pepper) do (   
@REM     for %%b in (0.01 0.03 0.05 0.07 0.09 0.10 0.12 0.15) do (
@REM             :: 激活环境并运行脚本
@REM             set OUTPUT_IMAGE=results_images/%%a_%%b.jpg
@REM             set VIDEO_PATH=results_videos/%%a_%%b.avi
@REM             echo 在 %%a 噪声类型下，噪声参数为 %%b
@REM             python %SCRIPT_PATH% --image_dir %IMAGE_DIR% --video_path results_videos/%%a_%%b.avi --output_image_path results_images/%%a_%%b.jpg --noise True --noise_type %%a --noise_param_1 %%b --noise_param_2 0
@REM     )
@REM )

echo 完成
pause
