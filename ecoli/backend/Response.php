<?php

namespace Ecoli;

class Response
{
	/**
	 * Response content type.
	 *
	 * @var string
	 */
	private $contentType = "text/html; charset=utf-8";
	
	/**
	 * Response HTTP code.
	 *
	 * @var int
	 */
	private $code = 200;

	/**
	 * Response content data.
	 *
	 * @var mixed
	 */
	private $data = null;

	/**
	 * Create new response object.
	 *
	 * @param  string  $data
	 * @param  int  $httpCode
	 * @param  string  $contentType
	 * @return void
	 */
	public function __construct($data, $httpCode = 200, $contentType = 'text/html; charset=utf-8')
	{
		$this->data = $data;
		$this->code = $httpCode;
		$this->contentType = $contentType;
	}

	/**
	 * Set content to JSON.
	 *
	 * @return Ecoli\Response
	 */
	public function makeJson()
	{
		$this->contentType = 'application/json';

		return $this;
	}

	/**
	 * Create a 404.
	 *
	 * @return Ecoli\Response
	 */
	public function makeNotFound()
	{
		$this->code = 404;

		return $this;
	}

	/**
	 * Set content data.
	 *
	 * @return Ecoli\Response
	 */
	public function data($data)
	{
		$this->data = $data;

		return $this;
	}

	/**
	 * Send response to client.
	 *
	 * @return void
	 */
	public function send()
	{
		if ($this->code == 404) {
			header('HTTP/1.1 404 Not Found');
		}

		if ($this->contentType == 'application/json') {
			header('Content-Type: application/json');
			header('Cache-Control: no-store, no-cache, must-revalidate, max-age=0');
			header('Cache-Control: post-check=0, pre-check=0', false);
			header('Pragma: no-cache');

			if (!is_null($this->data)) {
				$this->data = json_encode($this->data);
			}
		}

		if (!is_null($this->data)) {
			echo $this->data;
		}
	}

}
