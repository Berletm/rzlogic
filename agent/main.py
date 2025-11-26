from openai import OpenAI
from dotenv import load_dotenv
import os
import rzlogic
from utils import *

load_dotenv()

BASE_URL = os.getenv('BASE_URL')
DEEPSEEK_API_KEY = os.getenv('DEEPSEEK_API_KEY')
MODEL = os.getenv('MODEL')

client = OpenAI(api_key=DEEPSEEK_API_KEY, base_url=BASE_URL)

def text2premises(text: str) -> list[str]:
    response = client.chat.completions.create(model=MODEL,
                                              messages=[
                                                  {"role": "system", "content": agent_prompt},
                                                  {"role": "user", "content": text}],
                                              stream=False)
    
    return response.choices[0].message.content.split("\n")

def premises2text(text: str, raw_premises :list[str], resolved_premises: list[str], result: bool) -> str:
    if not result:
        return "Противоречие не найдено. "
    
    prompt = (
                "ИЗНАЧАЛЬНАЯ ЗАДАЧА: "  + text + "\n" + 
                "ИЗНАЧАЛЬНЫЕ ПОСЫЛКИ: " + "\n".join(raw_premises) + "\n"
                "ПОСЫЛКИ ПОЛУЧЕННЫЕ МЕТОДОМ РЕЗОЛЮЦИИ: " + "\n".join([f"{premise[0]} + {premise[1]} = {premise[2]}" for premise in resolved_premises])
            )
    
    response = client.chat.completions.create(model=MODEL,
                                              messages=[
                                                  {"role": "system", "content": interpretation_prompt},
                                                  {"role": "user", "content": prompt}],
                                              stream=False)
    
    return response.choices[0].message.content

def resolution(text: str) -> str:
    premises  = text2premises(text)
    res, history = rzlogic.make_resolution(premises)
    interpretation = premises2text(text, premises, history, res)
    return interpretation

def main():
    print(resolution(statement7))

    
if __name__ == "__main__":
    main()
