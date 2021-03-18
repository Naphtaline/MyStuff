from psutil import cpu_count
from threading import Lock
from concurrent.futures import ThreadPoolExecutor
import sqlite3

from utilitary import get_value_between_marker
from utilitary import download_at_url
from utilitary import retreive_info_from_page
from utilitary import add_name_variations

# TODO take url as parameters
# TODO take force threads count as parameter
# TODO take max article to fetch as parameter

debug = True

author_set_lock = Lock()
author_set = set()

title_set_lock = Lock()
title_set = set()

if debug == False:
    print('Begin program')

# gather_author_name_and_title_from_article
#   This is a threadsafe function
# args :
#       article_url : must be string
def gather_author_name_and_title_from_article(article_url):
    if debug == True:
        print('Enter Function : gather_author_name_and_title_from_article')

    author_name = retreive_info_from_page(article_url,
                                          '<p class=\'author\'>',
                                          '</p>',
                                          1)
    for author in author_name:
        author_set_lock.acquire()
        author_set.add(author)
        author_set_lock.release()

    title = retreive_info_from_page(article_url,
                                          '<h1 class="page-title center underline">',
                                          '</h1>',
                                          1)
    for value in title:
        title_set_lock.acquire()
        title_set.add(value)
        title_set_lock.release()

# retrieve_data_to_process
#   This will retrieve all necessary data with only 1 download 
#   for each resource.
def retrieve_data_to_process():
    if debug == True:
        print('Enter Function : retrieve_data_to_process')

    # we get the number of cpus ...
    cpu_number = cpu_count(logical=True)
    print(str(cpu_number) + ' cpu found !')
    # then we init the thread pool regarding that number
    executor = ThreadPoolExecutor(cpu_number)

    # get article links
    article_links = retreive_info_from_page('https://www.digitemis.com/blog',
                            '<a class="card-post" href="',
                            '\" title=\"', 0)
    print(str(len(article_links)) + ' articles found.')
    # here we feed the threadpool with all the articles
    for link in article_links:
        executor.submit(gather_author_name_and_title_from_article, (link))

    # we then shutdown the threadpool with wait=True arg to make
    # sure that all articles have been treated and are done
    executor.shutdown(True)

# handle_title_words
#   This will process title words data
# args :
#        result_dict : must be a set
def handle_title_words(result_dict):
    if debug == True:
        print('Enter Function : handle_title_words')
    
    word_to_remove = set()

    for value in title_set:
        value = value.lower()
        value_split = value.split(' ')

        for word in value_split:
            if word in result_dict:
                result_dict[word] = result_dict[word] + 1
                if result_dict[word] >= 3:
                    # word count >= 3, we remove it from word_to_remove
                    word_to_remove.discard(word)
            elif len(word) > 1 or word.isalpha():
                result_dict[word] = 1
                # each word's first entry is a candidate to be removed
                word_to_remove.add(word)

    # counting done, now we clean up the dict
    for word in word_to_remove:
        del result_dict[word]

# init_database
#   This will init the database
#   It should return the connection object
#   In case of error, return None
def init_database():
    if debug == True:
        print('Enter Function : init_database')
    try:
        sqlite_connection = sqlite3.connect('./test.db')
        print('Opened database successfully')

        sqlite_connection.execute(
            '''CREATE TABLE IF NOT EXISTS name_variations(
                    name    TEXT    NOT NULL
                    );''')
        sqlite_connection.execute(
            '''CREATE TABLE IF NOT EXISTS repetitve_title_words(
                    word        TEXT    NOT NULL,
                    repetition  INT     NOT NULL
                    );''')
    except sqlite3.Error as sqlite_error:
        print("An error occurred:", sqlite_error.args[0])
        return None
    return sqlite_connection

# insert_name
#   This will insert a name variation into the database
# args :
#       sqlite_connection   must be a sqlite connection
#       name                must be a string
def insert_name(sqlite_connection, name):
    try:
        statement = "INSERT INTO name_variations (name)\
                     VALUES ('" + name + "');"
        sqlite_connection.execute(statement);
    except sqlite3.Error as sqlite_error:
        print("An error occurred:", sqlite_error.args[0])

# insert_word
#   This will insert a word and its reptition umber into the database
# args :
#       sqlite_connection   must be a sqlite connection
#       word                must be a string
#       repetition          must be an int
def insert_word(sqlite_connection, word, repetition):
    try:
        statement = "INSERT INTO repetitve_title_words (word, repetition)\
                     VALUES ('" + word + "', " + str(repetition) + ");"
        sqlite_connection.execute(statement);
    except sqlite3.Error as sqlite_error:
        print("An error occurred:", sqlite_error.args[0])

# display_all_data
#   This is a debug function that will display all the data gathered
# args :
#        sqlite_connection  must be a sqlite connection
#        result_name_set    must be the result name set
#        result_dict        must be the result dict
def display_all_data(sqlite_connection, result_name_set, result_dict):
    print("//// result_name_set ////")
    print("result_name_set length = " + str(len(result_name_set)))
    print("//// result_name_set  SELECT from db ////")
    cursor = sqlite_connection.execute("SELECT rowid, * from name_variations")
    for row in cursor:
        print("id = " + str(row[0]) + " : name = " + row[1])

    print("//// result_words ////")
    print("result_dict length = " + str(len(result_dict)))
    for key, value in result_dict.items():
        print(key, value)
    print("//// result_words  SELECT from db ////")
    cursor = sqlite_connection.execute("SELECT rowid, *\
                                        from repetitve_title_words")
    for row in cursor:
        print(  "id = " + str(row[0])
                + " : word = "
                + row[1] + ", "
                + str(row[2]) + " times")

def main():
    if debug == True:
        print('Enter Function : main')

    sqlite_connection = init_database()
    if sqlite_connection is None:
        return
    print('Database init successful')

    # get all data requiered from internet
    name_set = retrieve_data_to_process()
    
    # handle the author data
    result_name_set = set()
    for author in author_set:
        add_name_variations(author, result_name_set)
    
    # handle the title data
    result_dict = dict()
    handle_title_words(result_dict)

    # we insert our name variation result to the database
    for name in result_name_set:
        insert_name(sqlite_connection, name)

    # we insert our word repetition result to the database
    for word, repetition in result_dict.items():
        insert_word(sqlite_connection, word, repetition)

    if debug == True:
        display_all_data(sqlite_connection, result_name_set, result_dict)


main()
