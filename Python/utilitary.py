import urllib.request

debug = True

def get_value_between_marker(str, first_marker, second_marker):
    index = str.find(first_marker)
    return_value = ""
    
    if index >= 0:
        index = index + len(first_marker)
        while True:
            # the "or" condition is here to prevent infite loop. We never know
            if str[index] == second_marker or index > 100000 : 
                break;
            else:
                return_value = return_value + str[index]
            index += 1
    return return_value

def download_at_url(url):
    if debug == True:
        print('Enter Function : downloadAtUrl')
        print('---------- Url :' + url)

    response = urllib.request.urlopen(url)
    return response
          #  match = get_value_between_marker(line.decode("utf-8"), 
           #                                  '<p class=\'author\'>',
            #                                 '<')

def retreive_info_from_page(url, first_marker, second_marker, match_amount = 0):
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

