from psutil import cpu_count
from threading import Lock
from concurrent.futures import ThreadPoolExecutor

from utilitary import get_value_between_marker
from utilitary import download_at_url
from utilitary import retreive_info_from_page


# TODO take url as parameters

debug = False

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

# add_name_variations
#   This function will add all the variants requiered from "name" to the
#   "result_name_set_ref" set
# args :
#       name                : must be a string
#       result_name_set_ref : must be a set
def add_name_variations(name, result_name_set_ref):
    name = name.lower()
    name_split = name.split(' ', 1)
    if len(name_split) < 2:
        return
    first_name = name_split[0]
    last_name = name_split[1]
    
    result_name_set_ref.add(last_name + '.' + first_name)
    result_name_set_ref.add(first_name + '.' + last_name)
    result_name_set_ref.add(str(first_name[0]) + '.' + last_name)
    result_name_set_ref.add(str(first_name[0]) + '_' + last_name)
    result_name_set_ref.add(last_name + '_' + str(first_name[0]))
    result_name_set_ref.add(first_name + '_' + last_name)
    result_name_set_ref.add(last_name + '_' + first_name)

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
    if debug == True:
        print("//// article_links ////")
        for item in article_links:
            print(item)
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
                # each word first entry is a candidate to be removed
                word_to_remove.add(word)

    # counting done, now we clean up the dict
    for word in word_to_remove:
        del result_dict[word]


def main():
    if debug == True:
        print('Enter Function : main')

    # get all data requiered from internet
    name_set = retrieve_data_to_process()
    
    # handle the author data
    result_name_set = set()
    for author in author_set:
        add_name_variations(author, result_name_set)
    
    # handle the title data
    result_dict = dict()
    handle_title_words(result_dict)

    if debug == True:
        print("//// result_name_set ////")
        for value in result_name_set:
            print(value)
        print("//// result_dict ////")
        for key, value in result_dict.items():
            print(key, value)
        print("result_name_set =" + str(len(result_name_set)))


main()
