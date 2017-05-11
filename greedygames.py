import json
import socket
import urllib
import urllib2
import oauth
import oauth2
from flask import Flask,render_template
from flask import request



def returnGoogleResult(searchfor):
  jsonGoogle = []
  data = {}
  data["q"] = searchfor
  data["key"] = "AIzaSyDkGfh9ORdwdSOO6jUKFjo6rJ1kSedOlw8"
  data["cx"] = "017576662512468239146:omuauf_lfve"
  url_values = urllib.urlencode(data)
  url = 'https://www.googleapis.com/customsearch/v1'
  finalurl= url + "?" + url_values
  print finalurl
  try:
      search_response = urllib2.urlopen(finalurl, None, 1) #timeout after 1 sec
      search_results = search_response.read()
      results = json.loads(search_results)
      data = results['items']
      count=0
      for each in data:
        # print each
        eachObj={'img_url':"",'text':""}
        eachObj['img_url']=each['link']
        eachObj['text']=each['title']
        jsonGoogle.append(eachObj)
        count=count+1
        if(count>=5):
          break
      return jsonGoogle
  except urllib2.URLError, e:
        print "Oops"
        return jsonGoogle
  except socket.timeout, e:
        print 'Oops, timed out for google'
        return jsonGoogle

#   print 'Total results: %s' % data['cursor']['estimatedResultCount']
#   hits = data['results']
#   print 'Top %d hits:' % len(hits)
#   for h in hits: print ' ', h['url']
#   print 'For more results, see %s' % data['cursor']['moreResultsUrl']
#


def returnDuckDuckGoResult(searchfor):
  jsonDuck = []
  data = {}
  data["q"] = searchfor
  data["format"] = "json"
  url_values = urllib.urlencode(data)
  url = 'http://api.duckduckgo.com'
  finalurl= url + "?" + url_values
  # print finalurl

  try:
      search_response = urllib2.urlopen(finalurl, None, 1) #timeout after 1 sec
      search_results = search_response.read()
      results = json.loads(search_results)
      data = results['RelatedTopics']
      # jsonData=json.loads(data)
      count=0
      for each in data:

        eachObj={'img_url':"",'text':""}

        try:

          eachObj['img_url']=each['FirstURL']
          eachObj['text']=each['Text']
          # print each
          count = count + 1
        except:
          continue
        jsonDuck .append(eachObj)
        if(count>=5):
          break
      # return json.dumps(jsonDuck )
      return jsonDuck
  except urllib2.URLError, e:
        print "Oops?"
        return jsonDuck
  except socket.timeout, e:
        print 'Oops, timed out for duck duck'
        return jsonDuck

# print returnDuckDuckGoResult('dog')





def oauth_req(url):
    consumer = oauth2.Consumer(key='H9nZNvUurbZKVv2jowbIN8OES', secret='z54ZkIQOmVr6EOGI1IpTRYeOv2n1jlK37rOMHPkQI4lT76XiXP')
    token = oauth2.Token(key= '715602450934202369-uukzkFRNNAE5rOUeuGmVRwZBLaQQ76t', secret='8rDlrADMfu3v0g0lcXtqYiqV9oloubEfbZLDvRNlvlv4D')
    client = oauth2.Client(consumer, token)

    resp, content = client.request(
    url,
    method="GET"
    )
    return content

def returnTwitterResult(query):
  searchResults = oauth_req('https://api.twitter.com/1.1/search/tweets.json?src=typd&q='+query)
  # print searchResults
  jsonTwitter = []
  results = json.loads(searchResults)
  data = results['statuses']
  # jsonData=json.loads(data)
  count = 0
  for each in data:
    # print each
    eachObj = {'img_url': "", 'text': ""}
    eachObj['img_url'] = each['source']
    eachObj['text'] = each['text']
    jsonTwitter.append(eachObj)
    count = count + 1
    if (count >= 5):
      break

  return jsonTwitter


def combined(keyword):
  result={'results':'','query':keyword}
  queries={'google':[],'twitter':[],'duckduckgo':[]}
  queries['google']=returnGoogleResult(keyword)
  queries['twitter']=returnTwitterResult(keyword)
  queries['duckduckgo']=returnDuckDuckGoResult(keyword)
  result['results']=queries

  return json.dumps(result)

# print
# print returnTwitterResult('dog')



app = Flask(__name__)

@app.route("/")

def index():
    return render_template("index.html")

@app.route("/success",methods=['POST'])

def success():

    if request.method=='POST':
        result=request.form["data"]
        return render_template("success.html",result = combined(result))

# @app.route('/')
# def returnJson():
#     print 'hello'
#
#     return combined(request.args.get('query'))


if __name__ == '__main__':
    app.run()
