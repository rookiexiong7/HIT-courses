import gradio as gr
from PIL import Image
import numpy as np
from PIL import Image
from inference import inference

def reset_state():
    return None,None,None,None,None

def predict(input, model):
    image = Image.fromarray(input)
    image.save("restored_image.jpg")
    result = inference("restored_image.jpg", model)
    image = Image.open('runs/detect/exp/restored_image.jpg')

    return result, image

def predict_reference(data):
    input = Image.open(data[0])
    input.save("restored_image.jpg")
    result = inference("restored_image.jpg", data[1])
    image = Image.open('runs/detect/exp/restored_image.jpg')
    return input, result, image, data[1]

with gr.Blocks(css=".gradio-container {background: url('file=4.jpg');background-size: cover;") as demo:
    gr.HTML("""<h1 align="center" style="black">基于YOLO的目标检测系统</h1>""")

    with gr.Row():
        with gr.Column():
            user_input = gr.Image(show_label=False, placeholder="Input...").style(container=False)
            result = gr.Textbox(placeholder="Results...")
        with gr.Column():
            output = gr.Image()
    with gr.Row():
        with gr.Column():
            with gr.Column(min_width=32, scale=1):
                submitBtn = gr.Button("上传", variant="primary")
            with gr.Column(min_width=32, scale=1):
                emptyBtn = gr.Button("清除")
        with gr.Column():
            weights = gr.Dropdown(
                ["yolov7-tiny.pt(default)", "yolov7-tiny.pt(strawberry)", "yolov7-tiny.pt(attention)"],
                label="Model", info="选择一个合适的模型")

    submitBtn.click(predict, [user_input, weights],
                    [result, output], show_progress=True)
    emptyBtn.click(reset_state, outputs=[result,user_input,output], show_progress=True)
    data = gr.Dataset(label='示例图像', components=["image", "textbox"],
                      samples=[['inference/images/horses.jpg','yolov7-tiny.pt(default)'],
                               ['inference/images/zidane.jpg','yolov7-tiny.pt(default)'],
                               ['inference/test/1.jpg','yolov7-tiny.pt(attention)']])
    data.click(predict_reference, data,
                    [user_input, result, output, weights], show_progress=True)

demo.queue().launch(share=False, inbrowser=True)