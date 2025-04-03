from openai import OpenAI

# 初始化客户端
client = OpenAI(
    api_key="your-api-key-here",  
    base_url="https://api.deepseek.com",
)

try:
    # 创建对话补全请求
    response = client.chat.completions.create(
        model="deepseek-chat",  # 指定模型（可选：deepseek-chat 或 deepseek-reasoner）
        messages=[
            # system prompt
            {"role": "system", "content": "你是我调用的deepseekAPI，出于程序需求，说的话最好不要超过30个字！还得保持和蔼可亲"},
            # 实际提问
            {"role": "user", "content": "你好呀"},
        ],
        stream=False,  # 是否使用流式输出（True为流式，False为完整返回）
        temperature=0.7,  # 控制生成随机性（0-2，越高越随机）
        max_tokens=500,  # 限制生成最大token数量
    )

    # 提取并打印回复内容
    print("DeepSeek回复：")
    print(response.choices[0].message.content)
    
    # 以下是可选的调试信息打印
    print("\n调试信息：")
    print(f"消耗tokens: {response.usage.total_tokens}")
    print(f"请求ID: {response.id}")

except Exception as e:
    print(f"API调用失败: {str(e)}")