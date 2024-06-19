import datetime

def zodiac_sign(date):
	day = int(date.strftime("%d"))
	month = date.strftime("%B").lower()
	astro_sign = ""
	if month == 'december': 
		astro_sign = 'Sagittarius' if (day < 22) else 'Capricorn'
	elif month == 'january': 
		astro_sign = 'Capricorn' if (day < 20) else 'Aquarius'
	elif month == 'february': 
		astro_sign = 'Aquarius' if (day < 19) else 'Pisces'
	elif month == 'march': 
		astro_sign = 'Pisces' if (day < 21) else 'Aries'
	elif month == 'april': 
		astro_sign = 'Aries' if (day < 20) else 'Taurus'
	elif month == 'may': 
		astro_sign = 'Taurus' if (day < 21) else 'Gemini'
	elif month == 'june': 
		astro_sign = 'Gemini' if (day < 21) else 'Cancer'
	elif month == 'july': 
		astro_sign = 'Cancer' if (day < 23) else 'Leo'
	elif month == 'august': 
		astro_sign = 'Leo' if (day < 23) else 'Virgo'
	elif month == 'september': 
		astro_sign = 'Virgo' if (day < 23) else 'Libra'
	elif month == 'october': 
		astro_sign = 'Libra' if (day < 23) else 'Scorpio'
	elif month == 'november': 
		astro_sign = 'Scorpio' if (day < 22) else 'Sagittarius'
	return astro_sign

def chinese_zodiac(date):
	signs = ["Monkey", "Rooster", "Dog", "Pig", "Rat", "Ox", "Tiger", "Rabbit", "Dragon", "Snake", "Horse", "Goat"]
	year = int(date.strftime("%Y"))
	return signs[year % 12]

values = input().split("&")
name = None
date_of_birth = None
for field in values:
	if field.startswith("name="):
		name = field[5:]
	if field.startswith("date-of-birth="):
		date_of_birth = datetime.date(int(field[20:24]), int(field[17:19]), int(field[14:16]))

print("Content-Type: text/plain\n")

print(f"Hi {name}, I'm going to tell some facts about yourself\n")
print(f"You were born on a {date_of_birth.strftime('%A')}")
print(f"Your Zodiac sign is {zodiac_sign(date_of_birth)}")
print(f"You were born in the Chinese year of the {chinese_zodiac(date_of_birth)}")
print(f"Your next birthday is in {int(str(datetime.date(datetime.date.today().year + 1, date_of_birth.month, date_of_birth.day) - datetime.date.today()).replace('days, 0:00:00', '')) % 365} days")
print(f"Your are approximately {int((datetime.date.today() - date_of_birth).total_seconds() / 3600):,} hours old")