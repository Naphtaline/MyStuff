from psutil import cpu_count
from threading import Lock
from concurrent.futures import ThreadPoolExecutor

from utilitary import get_value_between_marker
from utilitary import download_at_url
from utilitary import retreive_info_from_page


# TODO take url as parameters

debug = True

author_set_lock = Lock()
author_set = set()

if debug == False:
    print('Begin program')

# gather_author_name_from_article
# args :
#       article_url : must be string
def gather_author_name_from_article(article_url):
    author_name = retreive_info_from_page(article_url,
                                          '<p class=\'author\'>',
                                          '<',
                                          1)
    for author in author_name:
#        if debug == True:
#            print(author)
        author_set_lock.acquire()
        author_set.add(author)
        author_set_lock.release()

def main():
    if debug == True:
        print('Enter Function : main')

# we get the number of cpus ...
    cpu_number = cpu_count(logical=True)
    print(str(cpu_number) + ' cpu found !')
# then we init the thread pool regarding that number
    executor = ThreadPoolExecutor(cpu_number)

# retrieve the main page to scrap info out of
    result = 0
    total_articles = 0

    article_links = retreive_info_from_page('https://www.digitemis.com/blog',
                            '<a class="card-post" href="',
                            '"', 5)
    total_articles = len(article_links)
    print(str(total_articles) + ' articles found.')

    # here we feed the threadpool with all the articles
    for link in article_links:
        executor.submit(gather_author_name_from_article, (link))
    # we then shutdown the threadpool with wait=True arg to make
    # sure that all articles have been treated and are done
    executor.shutdown(True)

    print('show set :')
    for author in author_set:
        print(author)


main()
