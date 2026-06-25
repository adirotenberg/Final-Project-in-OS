# direction on how to move to a tag and create a new branch from there

-- in terminal:
git clone https://github.com/adirotenberg/Final-Project-in-OS.git

cd Final-Project-in-OS

# before you run project (inside project folder):
make install-raylib

git fetch --tags

git tag

git checkout TAG_NAME

git checkout -b exam_a/מס תעודת זהות

# to open vscode to edit the code
code .

-- edit files -- 

-- in terminal:
git add .

git commit -m "your message"

git push -u origin exam_a/מס תעודת זהות