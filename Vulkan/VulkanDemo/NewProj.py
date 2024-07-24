import os
import shutil
import argparse

cur_folder_path = os.path.dirname(os.path.abspath(__file__))

def rename_file(old_name, new_name):
    try:
        os.rename(old_name, new_name)
        print(f"文件重命名成功：{old_name} -> {new_name}")
    except OSError as e:
        print(f"无法重命名文件：{e}")
        
        
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Copy and rename a project')
    parser.add_argument('new_proj_name', type=str, help='New Project Name')
    args = parser.parse_args()
    
    # 模板项目路径
    OriginalProjectName = "Template"
    source_project_path = os.path.join(cur_folder_path, OriginalProjectName)

    # 新项目路径
    NewProjectName = args.new_proj_name
    new_project_path = os.path.join(cur_folder_path, NewProjectName)
    
    # 复制项目文件夹
    shutil.copytree(source_project_path, new_project_path)
    
    # 更新项目文件夹内的项目文件名称
    for root, _, files in os.walk(new_project_path):
        for old_file_name in files:
            old_file_path = os.path.join(root, old_file_name)
            # 修改内容
            if ".vcxproj" in old_file_name:
                with open(old_file_path, 'r', encoding="utf8") as file:
                    lines = file.readlines()
                with open(old_file_path, 'w', encoding="utf8") as file:
                    for line in lines:
                        new_line = line.replace(OriginalProjectName, NewProjectName)
                        file.write(new_line)
                # 改名
                new_file_name = old_file_name.replace(OriginalProjectName, NewProjectName)
                new_file_path = os.path.join(root, new_file_name)
                rename_file(old_file_path, new_file_path)
    
