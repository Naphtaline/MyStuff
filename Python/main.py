#%%
#import psutil
import random
import threading
from concurrent.futures import ThreadPoolExecutor
from utilitary import get_value_between_marker
from utilitary import download_at_url
from utilitary import retreive_info_from_page


# TODO take url as parameters
# TODO multithread things Ncpu = psutil.cpu_count(logical=True)
debug = True

if debug == True:
    print('Begin program')

def main():
    if debug == True:
        random.seed()
        print('Enter Function : main')

# we get the number of cpus ...
    #cpu_number = psutil.cpu_count(logical=True)
# then we init the thread pool regarding that number
    executor = ThreadPoolExecutor(5)

# retrieve the main page to scrap info out of
    result = 0
    #autor_list_lock =
    article_links = retreive_info_from_page('https://www.digitemis.com/blog',
                            '<a class="card-post" href="',
                            '"', 5)
    print('articles found : ' + str(len(article_links)))
   # for link in article_links
   #    executor.submit(retreive_info_from_page
    return
    while True:
        line = downloaded.readline()
        if debug == True and result > 10:
            break
        if line:
            match = get_value_between_marker(line.decode("utf-8"), 
                                             '<a class="card-post" href="',
                                             '"')
            if match != "":
                result += 1
                executor.submit(retreive_info_from_page, match)
        else:
            break
    if debug == True:
        print('match for articles = ' + str(result))


main()


