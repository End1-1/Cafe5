import xml.etree.ElementTree as ET
from googletrans import Translator
import time

# НАСТРОЙКИ
file_path = 'Shop.ts'
target_lang = 'hy' # 'hy' для армянского, 'en' для английского
translator = Translator()

tree = ET.parse(file_path)
root = tree.getroot()

print("Начинаю перевод... Погнали!")

count = 0
for context in root.findall('context'):
    context_name = context.find('name').text
    for message in context.findall('message'):
        source = message.find('source').text
        translation = message.find('translation')

        # Переводим только если перевод пустой или помечен как unfinished
        if translation.text is None or translation.get('type') == 'unfinished':
            try:
                # Небольшая пауза, чтобы Google не забанил за 1800 строк сразу
                if count % 10 == 0:
                    time.sleep(1)
                
                result = translator.translate(source, dest=target_lang)
                translation.text = result.text
                # Убираем пометку unfinished, чтобы Linguist видел их как готовые
                if 'type' in translation.attrib:
                    del translation.attrib['type']
                
                count += 1
                print(f"[{count}] {source} -> {result.text}")
            except Exception as e:
                print(f"Ошибка на строке '{source}': {e}")
                time.sleep(5) # Если ошибка, ждем подольше

tree.write(file_path, encoding='utf-8', xml_declaration=True)
print(f"--- ГОТОВО! Переведено {count} строк. Загружай в Linguist! ---")