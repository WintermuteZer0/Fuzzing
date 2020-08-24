#!/usr/bin/python
import sys
import os 
sys.path = ["../"] + sys.path
import CppHeaderParser
import sqlite3

def get_target_headers(filepath):
    targets=[]
    for file in os.listdir(filepath):
        if file.endswith(".h"):
            targets.append(os.path.join(filepath, file))
    return targets


# Data Structure format 
# 
# {header_file:{ function_proto:{param_name:type},...}}
#
def get_functions(file):
    func_dict = {}
    try:
        cppHeader = CppHeaderParser.CppHeader(file)
    except CppHeaderParser.CppParseError as e:
        print(e)
        return {}
        

    for f in cppHeader.functions:
        param_list = f['parameters']
        param_dict={}
        for param in param_list:
            if param['name'] in param_dict:
                continue
            else:
                param_dict[param['name']] = param['raw_type']
        if f['debug'] in func_dict:
            continue
        else:
            func_dict[f['debug']]=param_dict
    return func_dict


def keyword_search(keyword,searchDict):
    #Search in function 
    #print(searchDict.keys())
    if hasattr(searchDict,'iteritems'):
        for k, v in searchDict.iteritems():
            if keyword in k :
                yield k
            if isinstance(v, dict):
                for result in keyword_search(keyword, v):
                    yield result
            elif isinstance(v, list):
                for d in v:
                    for result in keyword_search(keyword, d):
                        yield result

    #return [v for v in searchDict.values() if keyword in v ] 

########################################################################
DEFAULT_PATH = os.path.join(os.path.dirname(__file__), 'api_searcher.sqlite3')

CREATE_PROTO_TABLE1 = """
CREATE TABLE Prototypes (
    header_file text PRIMARY KEY,
    proto text NOT NULL,
    FOREIGN KEY (parameter) REFERENCES Parameters () 
    )"""
CREATE_PROTO_TABLE2= """
CREATE TABLE Prototypes (
    header_file text PRIMARY KEY,
    proto text NOT NULL 
    )"""
CREATE_PARAM_TABLE = """
CREATE TABLE Parameters (
    ID integer PRIMARY KEY,
    Name text NOT NULL,
    Type text NOT NULL
)"""

INSERT_PROTO_ENTRY ="""INSERT INTO Prototypes (header_file,proto) as (?,?) """

SEARCH_PROTO_KEYWORD = """ SELECT * FROM Prototypes WHERE proto LIKE %?%"""

# Create SQLlite db connection 
def db_connect(file=DEFAULT_PATH):
    con = sqllite.connect(file)
    return con

def insert_prototype(con, header, function):
     cur =con.cursor()
    try:
        # Loop through prototypes  in given header file and execute INSERTS 
        for prototype in function:
            cur.execute(INSERT_PROTO_ENTRY, header,prototype)
        con.commit()
        return cur.lastrowid
    except:
        # Rolback on error
        con.rollback()
        raise RuntimeError('Uh Oh Failed Commit....')


def create_database(file):
    from db_utils import db_connect
    con = db_connect(file)
    cur = con.cursor()
    cur.execute(CREATE_PROTO_TABLE2)
    print('[>>] API Database successfully created!')


########################################

def parse_headers():
    ## TODO - Add in optional support for user prompt for directory location
    files = get_target_headers('C:\Users\IEUser\Desktop\TargetFiles')
    funcDict={}
    for file in files:
        print("[>>>] Procesing:{} ".format(file))
        if file not in funcDict:
            funcDict[file]=get_functions(file)
        else:
            continue
    return funcDict


def main():
   
    db_file = input('[>>>] Please enter an existing DB location: ')
    if os.path.exists(db_file):
        con = db_connect(db_file)
        if con:
            keyword = input('[>>>] Please enter a search term: ')
    else:
        create_database(file)
        in_mem = parse_headers()
    
    
    
    
    
    
    
    
    for result in keyword_search(str(keyword),funcDict):
        print(result)

if __name__ == "__main__":
    main()
