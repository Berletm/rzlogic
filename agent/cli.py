from enum import Enum
from utils import *

problems = [statement1, statement2, statement3, statement4, statement5, statement6, statement7]

class State(Enum):
    SELECTION_STATE  = 4
    OUTPUT_STATE     = 5
    EXIT_STATE       = 6
    
    TEST_PROBLEMS    = 1
    OWN_PROBLEM      = 2
    EXIT             = 3

def mainloop() -> None:
    state = State.SELECTION_STATE
    
    while True:
        match (state):
            case State.SELECTION_STATE:
                os.system('clear')
                print("1 - Выбрать задачу из тестового набора.")
                print("2 - Ввести задачу самому.")
                print("3 - Выход.")
                
                k = input("Выберите следующее действие: ").strip()
                
                if k.isdigit():
                    choice = int(k)
                    if choice == 1:
                        state = State.TEST_PROBLEMS
                    elif choice == 2:
                        state = State.OWN_PROBLEM
                    elif choice == 3:
                        state = State.EXIT_STATE
                    else:
                        state = State.SELECTION_STATE
                else:
                    state = State.SELECTION_STATE
                    
            case State.TEST_PROBLEMS:
                os.system('clear')
                print("\n".join([f"{i}. {problem}" for i, problem in enumerate(problems)]))
                n = input("Выберите номер задачи: ").strip()
                
                if n.isdigit() and 0 <= int(n) < len(problems):
                    os.system("clear")
                    print("Текст задачи: ", problems[int(n)])
                    print("Вызов метода резолюции...")
                    print(resolution(problems[int(n)]))
                else:
                    print("Введите корректный номер.")
                
                state = State.OUTPUT_STATE
                    
            case State.OWN_PROBLEM:
                os.system('clear')
                text = ""
                
                while True:
                    print("1 для завершения ввода.")
                    premise = input("Введите предложение: ")
                    
                    if premise == "1":
                        break
                    
                    text += premise + ". "
                
                os.system("clear")
                print("Текст задачи: ", text)
                print("Вызов метода резолюции...")
                print(resolution(text))
                state = State.OUTPUT_STATE  
            
            case State.OUTPUT_STATE:
                input("Нажмите Enter, чтобы продолжить.")
                state = State.SELECTION_STATE
            
            case State.EXIT_STATE:
                break
            
            case _:
                os.system('clear')
                print("Введите корректное действие.")
                state = State.SELECTION_STATE
        
        