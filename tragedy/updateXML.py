# coding=utf-8

import os, glob, time

def search_all_files_return_by_time_reversed(path:str, reverse:bool=True):
    return sorted(
    	glob.glob(os.path.join(path, "*")), 
    	key=lambda x: os.path.getmtime(x),
    	reverse=reverse
    )

def generate_node(path_pre:str, path_name:str):
	ans = search_all_files_return_by_time_reversed(path_pre+path_name)
	return {path_name: ans}
def main():
	ans_dict = {}
	ans_dict.update(generate_node("./", "extra_md"))
	file_name = "./py_extra.xml"
	with open(file_name, "wb") as file_object:
		file_object.write(b"<?xml version=\"1.0\" encoding=\"UTF-8\" ?> \n<r0_qaq>\n")
		for it_k, it_v in ans_dict.items():
			file_object.write((f"<{it_k}>\n").encode("utf-8"))
			for it in it_v:
				if ".bak" in it or  ".py" in it:
					continue
				if ".md" not in it:
					continue
				file_object.write(b"<md>")
				file_object.write(it.encode("utf-8"))
				file_object.write(b"</md>\n")
			file_object.write((f"</{it_k}>\n").encode("utf-8"))
		file_object.write(b"</r0_qaq>")
if __name__ == '__main__':
	main()
