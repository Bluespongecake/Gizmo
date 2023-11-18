import requests
from bs4 import BeautifulSoup
import re

def get_questions(url):
    
    page = requests.get(url)
    soup = BeautifulSoup(page.content, 'html.parser')
    
    content = soup.body.find_all(text = re.compile('var FB'))
    
    match = re.findall('[,]["][\w\s]+["][,]', str(content))
    #It will match all the questions in the form
    question_strings = [x.strip('"') for x in match]
    
    match_ids = re.findall('(?<=\[\[)(\d+)', str(content))
    #It will find all the numbers in the content
    question_ids = ['entry.' + x for x in match_ids[1:]]
    #It will leave the first numbers (they are not the ids)
    return question_ids
    
# Below are only for when you want to know the form fills with their corresponding entry ids
#    questions = dict(zip(question_strings, question_ids))    
#    return questions


def send_answers(url, fname, lname, grade, section, subject): #arrange this as per your form requirements
    
    ids = get_questions(url)
    
    answers = [fname, lname, grade, section, subject]
    response = dict(zip(ids, answers))
    
    if 'viewform' in url:
        s = url.index('viewform') 
        response_url = url.replace(url[s::], 'formResponse?')
        
    try:
        r = requests.post(response_url, response)
        if r.status_code == 200:
            return '[!] Attendence posted !'
        #In case an error happens, it will raise an exception
        else:
            raise Exception

    #After raising the exception it will retry to submit using url reconstruction with prefilled values
    except:
        try:
            ans_list = [x + '=' + y for x, y in zip(ids, answers)]
            
            for i in range(0, len(ans_list)):
                response_url += ans_list[i]
                response_url += '&'
                
            response_url.strip("&")    
            r = requests.get(response_url)
            status = r.status_code
            
            if status == 200:
                return '[!] Attendance sent !'
            else:
                raise Exception
        #If still an error happens, it will print out a message.
        except:
            return '[!] Attendance not sent !'
                

url = 'https://docs.google.com/forms/d/e/1FAIpQLSci7l0T4Yy6G8MuXQdKGC6gBIa3p-jv0fesOgSzKPRdqjf1Zw/viewform'

fname = 'Your first name here'
lname = 'Your last name here'
grade = 'Your grade here'
section = 'Section here'
subject = 'Enter subject'

print(send_answers(url, fname, lname, grade, section, subject))