from openai import OpenAI
from dotenv import load_dotenv
import os

load_dotenv()

BASE_URL = os.getenv('BASE_URL')
DEEPSEEK_API_KEY = os.getenv('DEEPSEEK_API_KEY')
MODEL = os.getenv('MODEL')

client = OpenAI(api_key=DEEPSEEK_API_KEY, base_url=BASE_URL)

agent_prompt = (
    "Ты умный математик, работающий в области математической логики. "
    "Тебе дают текстовую задачу на русском языке. "
    "Тебе нужно её понять и преобразовать в набор посылок логики предикатов. "

    "Используй предикаты типа Human(x), Loves(x, y) и другие в таком же стиле в зависимости от задачи. "
    "Используй кванторы forall, exists. "
    "Логические операторы: implies (импликация), not (отрицание), and (конъюнкция), or (дизъюнкция). "

    "**ВНИМАНИЕ: Не используй математические символы `∀`, `∃`, `→`, `¬`, `∧`, `∨`. Всегда используй только слова: `forall`, `exists`, `implies`, `not`, `and`, `or`.**"

    "**Используй строго префиксную нотацию (оператор в начале). Например: `(implies (P x) (Q x))`, а не `(P(x) → Q(x))`.**"
    "Аргументы в предикатах пиши без пробела, например `(Human x)`, а не `(Human (x))`."

    "Выведи ТОЛЬКО посылки, где каждая посылка с новой строки, без нумерации и без любых других символов. "
    "Пример формата вывода (измени предикаты под свою задачу):\n"
    "(forall x (implies (P x) (Q x)))\n"
    "(P a)\n"
    "(not (Q y))"
)

statement1 = "Все люди - смертны. Сократ - человек. Сократ - смертен."
statement2 = "Некоторые пациенты любят всех докторов. Ни один пациент не любит знахарей. Никакой доктор - не знахарь."

def main():
    response = client.chat.completions.create(model=MODEL,
                                              messages=[
                                                  {"role": "system", "content": agent_prompt},
                                                  {"role": "user", "content": statement2}],
                                              stream=False)

    print(response.choices[0].message.content)

if __name__ == "__main__":
    main()
