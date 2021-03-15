import urllib.request
import unidecode

debug = False

# get_value_between_marker
#   Will return a string containing the value (if markers are present) between
#   first_marker and second_marker.
# args :
#       str             : must be string
#       first_marker    : must be string
#       first_marker    : must be string
def get_value_between_marker(string, first_marker, second_marker):
    return_value = ""
    index_first = string.find(first_marker)
    index_second = string.find(second_marker)

    if index_first >= 0 and index_second > 0:
        if index_first < index_second:
            index_first = index_first + len(first_marker)
            while index_first < index_second:
                # the "or" condition is here to prevent infite loop.
                # We never know
                return_value = return_value + string[index_first]
                index_first += 1
    return return_value

# download_at_url
#   Will return a byte array containing the value dowloaded from "url" arg
#   address.
# args :
#       url             : must be string
def download_at_url(url):
    if debug == True:
        print('Enter Function : downloadAtUrl')
        print('---------- Url :' + url)

    response = urllib.request.urlopen(url)
    return response

# retreive_info_from_page
#   Will return a set (so no duplicates) containing string of all values that
#   are contains between "first_marker" and "second_marker" in a downloaded
#   page specified by "url" address.
#   
#   "match_amount" is an optionnal arg (defaut value 0 used as infinite here)
#   can be used to limit the amount of result we want. (thus possibly saving
#   performance by not over evaluating the downloaded resource)
# args :
#       url             : must be string
#       first_marker    : must be string
#       second_marker   : must be string
#       match_amount    : (optionnal) must be int
def retreive_info_from_page(url, first_marker,
                            second_marker, match_amount = 0):
    if debug == True:
        print('Enter Function : retreive_info_from_page')
        print('---------- Url :' + url)

    downloaded = download_at_url(url)
    set_to_return = set()
    counter = 0
    while True:
        line = downloaded.readline()
        if line:
            match = get_value_between_marker(line.decode("utf-8"), 
                                             first_marker,
                                             second_marker)
            if match != "":
                # convert utf-8 to non-extended ascii text
                match = unidecode.unidecode(match)
                set_to_return.add(match)
                if match_amount > 0:
                    counter += 1
            if match_amount > 0 and counter >= match_amount:
                break
        else :
            break

    return set_to_return

