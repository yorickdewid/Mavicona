#ifndef ADDITIONAL_INDEX_H
#define ADDITIONAL_INDEX_H

class AbstractAdditionalIndex {
	std::vector<std::string> commonIndexKeywords = {{
		"name", "author", "owner", "user", "username",
		"date", "datetime", "timestamp", "locale",
		"age", "type", "classification", "category",
		"group", "set", "rating", "result", "status",
		"location", "language", "place", "target",
		"url", "address", "path", "description",
		"remark", "subject", "comment",	"note",	"tag",
		"color", "file", "width", "height", "time", "shape",
		"size", "texture", "source"
	}};

  public:
	bool matchKeyword(std::string keyword) {
		std::vector<std::string>::iterator it = find(commonIndexKeywords.begin(), commonIndexKeywords.end(), keyword);
		if (it == commonIndexKeywords.end())
			return false;
		
		return true;
	}

};

#endif // ADDITIONAL_INDEX_H
