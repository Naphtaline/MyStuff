#%%
import urllib.request

# TODO take url as parameters
# TODO multithread things Ncpu = psutil.cpu_count(logical=True)
debug = True

if debug == True:
    print('Begin program')

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

def download_at_url(Url):
    if debug == True:
        print('Enter Function : downloadAtUrl')
        print('---------- Url :' + Url)

    response = urllib.request.urlopen(Url)
    return response

def main():
    if debug == True:
        print('Enter Function : main')

    downloaded = download_at_url('https://www.digitemis.com/blog')
    result = 0
    while True:
        line = downloaded.readline()
        if line:
            match = get_value_between_marker(line.decode("utf-8"), 
                                             '<a class="card-post" href="',
                                             '"')
            if match != "":
                if debug == True:
                    print(match)
                result += 1
        else:
            break
    print('result = ' + str(result))


main()


