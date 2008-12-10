class LoadCss
	PATH = "public/stylesheets"

	def self.load( file, keys = nil )
		result = ''
		data = File.open("#{LoadCss::PATH}/#{file}").readlines.join(' ')

			#Trim down teh css file
	data = data.gsub(/\/\*.*?\*\//, ' ').gsub(/[\t\n ]+/, ' ').gsub(/[ ]*([{}:;,])[ ]*/, '\1')

			#Only add things the user asked for
		if keys.is_a? Array 
			hash = Hash.new
			keys.each { |k| hash[k.to_s] = true; hash[".#{k.to_s}"] = true }
			data.split(/\}/).each do |v|
				v.split(/\{/)[0].split(/[ ]+/).each do |k|
					if hash.has_key?(k)
						result += "#{v}}"
						break
					end
				end
			end 
		else
			result = data
		end

		return (result.nil?)? '': "<style type=\"text/css\">#{result}</style>"
	end
end
