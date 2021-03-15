import urllib.request

debug = False

# get_value_between_marker
#   Will return a string containing the value (if markers are present) between
#   first_marker and second_marker.
# args :
#       str             : must be string
#       first_marker    : must be string
#       first_marker    : must be string
def get_value_between_marker(str, first_marker, second_marker):
    index = str.find(first_marker)
    return_value = ""
    
    if index >= 0:
        index = index + len(first_marker)
        while True:
            # the "or" condition is here to prevent infite loop.
            # We never know
            if str[index] == second_marker or index > 100000 : 
                break;
            else:
                return_value = return_value + str[index]
            index += 1
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
#       Will return a set (so no duplicates) containing string of all values that
#   are contains between "first_marker" and "second_marker" in a downloaded
#   page specified by "url" address.
#       "match_amount" is an optionnal arg (defaut value 0 used as infinite here)
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
                set_to_return.add(match)
                if match_amount > 0:
                    counter += 1
                if debug == True:
                    print(match)
            if match_amount > 0 and counter >= match_amount:
                break
        else :
            break
    return set_to_return

